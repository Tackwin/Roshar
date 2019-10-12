
#ifdef WEB

#define SDL_MAIN_HANDLED

#include <GLES3/gl3.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>
#include "Game.hpp"

#include "imgui_impl_web.hpp"
#include "External/web/imgui.h"
#include "OS/RealTimeIO.hpp"
#include "Managers/AssetsManager.hpp"
#include "Graphic/Graphics.hpp"
#include "Graphic/FrameBuffer.hpp"

io::Keyboard_State emscripten_keyboard_state;
constexpr Vector2u Gl_Buffer_Size = { 1280, 720 };

bool init_gl_context() noexcept;
void setup_controls_callback() noexcept;
void loop() noexcept;
void render_orders(render::Orders& orders) noexcept;

extern void update_game(std::uint64_t dt) noexcept;
extern void render_game(render::Orders& orders) noexcept;

int main() {
	if (!init_gl_context()) {
		printf("Error on context init.\n");
		return 1;
	}

	setup_controls_callback();

	FunImGui::init();

	asset::Store.monitor_path("shaders/");
	asset::Store.monitor_path("textures/");

	asset::Store.load_from_config("assets/config.json");
	asset::Store.load_known_shaders();
	asset::Store.load_known_textures();

	Environment.window_width = 1280;
	Environment.window_height = 720;

	Game local_game;
	game = &local_game;
	game->load_start_config();

	emscripten_set_main_loop(loop, 0, 1);

	return 0;
}

void loop() noexcept{
	static render::Orders orders;
	static bool Show_Test_Window = true;
	static auto last_time_frame = microseconds();
	static float max_dt = 0;
	static size_t last_dt_count = 200;
	static std::vector<float> last_dt;

	std::uint64_t dt = microseconds() - last_time_frame;
	last_time_frame = microseconds();

	Main_Mutex.lock();
	defer{ Main_Mutex.unlock(); };


	FunImGui::BeginFrame();

	ImGui::Begin("Environment");
	ImGui::InputInt("Drag angle step", &Environment.drag_angle_step);
	ImGui::InputFloat("Gather speed", &Environment.gather_speed);
	ImGui::InputFloat("Gather step", &Environment.gather_step);
	ImGui::InputFloat("Drag", &Environment.drag);
	ImGui::InputFloat("Binding range", &Environment.binding_range);
	ImGui::InputFloat("Gravity", &Environment.gravity);
	ImGui::InputFloat("Dead velocity", &Environment.dead_velocity);
	ImGui::InputFloat("Speed up", &Environment.speed_up_step);
	ImGui::InputFloat("Offset", &Environment.offset);
	ImGui::Checkbox("Debug Input", &Environment.debug_input);


	int x = (int)Environment.debug_framebuffer;

	ImGui::ListBox(
		"Debug Framebuffer",
		&x,
		[](void*, int i, const char** out) {
			if (i >= (int)Debug_Framebuffer::Count) return false;
			switch ((Debug_Framebuffer)i) {
			case Debug_Framebuffer::Albedo:    *out = "Albedo";  break;
			case Debug_Framebuffer::Default:   *out = "Default"; break;
			case Debug_Framebuffer::Depth:     *out = "Depth";   break;
			case Debug_Framebuffer::Normal:    *out = "Normal";  break;
			case Debug_Framebuffer::Position:  *out = "Position"; break;
			default: return false;
			}
			return true;
		},
		nullptr,
			(int)Debug_Framebuffer::Count
			);
	Environment.debug_framebuffer = (Debug_Framebuffer)x;

	ImGui::End();

	ImGui::Begin("Update debug");
	update_game(dt);
	ImGui::End();


	auto t_start = seconds();
	render_game(orders);
	auto cpu_render_time = seconds() - t_start;

	render_orders(orders);


	orders.clear();

	ImGui::Begin("Perf");

	//float avg = 0;
	//for (auto y : last_dt) avg += y;
	//avg /= last_dt_count;
//
	//ImGui::Text(
	//	"current dt: %llu ms, avg(%lu): %llu ms, max: %llu",
	//	(unsigned long long)(dt / 1000),
	//	last_dt_count,
	//	(unsigned long long)(avg / 1000),
	//	(unsigned long long)(max_dt / 1000)
	//);
	//ImGui::Text("Fps: %lu", (size_t)(1'000'000.0 / dt));
	ImGui::Text("Cpu render time: %.3f ms.", cpu_render_time * 1'000);
	ImGui::Text("dt time: %.3f ms.", dt / 1000);

	ImGui::End();

	ImGui::Render();	
}


void render_orders(render::Orders& orders) noexcept {
	static Texture_Buffer texture_target{ Gl_Buffer_Size };
	static G_Buffer   g_buffer{ Gl_Buffer_Size };
	static HDR_Buffer hdr_buffer{ Gl_Buffer_Size };
	std::vector<render::View_Info> view_stack;

	glViewport(0, 0, (GLsizei)Gl_Buffer_Size.x, (GLsizei)Gl_Buffer_Size.y);

	g_buffer.set_active();
	g_buffer.clear({ 0.6, 0.3, 0.4, 1. });
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < orders.objects.size(); ++i) {
		auto& x = orders.objects[i];

		switch (x.kind) {
		case render::Order::Kind::View_Pop: {
			assert(!view_stack.empty());
			view_stack.pop_back();
			if (!view_stack.empty()) render::current_view = view_stack.back();
			break;
		}
		case render::Order::Kind::View_Push: {
			view_stack.push_back(x.view);
			render::current_view = view_stack.back();
			break;
		}
		case render::Order::Kind::Sprite:
			render::immediate(x.sprite);
			break;
		case render::Order::Kind::Rectangle:
			render::immediate(x.rectangle);
			break;
		case render::Order::Kind::Circle:
			render::immediate(x.circle);
			break;
		case render::Order::Kind::Arrow:
			render::immediate(x.arrow);
			break;
		case render::Order::Kind::Line:
			render::immediate(x.line);
			break;
		default: assert("Logic error.");
		}
	}

	hdr_buffer.set_active();
	g_buffer.set_active_texture();
	glClearColor(0.6f, 0.3f, 0.4f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	std::vector<render::Ambient_Light> ambient_lights;
	ambient_lights.push_back({ .color = {1, 1, 1, 1},.intensity = 1 });

	size_t point_light_idx = 0;
	for (size_t j = 0; j < orders.lights.size(); ++j) {
		auto& x = orders.lights[j];
		switch (x.kind) {
		case render::Order::Kind::Point_Light:
			x.point_light.idx = point_light_idx++;
			render::immediate(x.point_light);
			break;
		case render::Order::Kind::Ambient_Light_Push:
			ambient_lights.push_back(x.ambient_light);
			break;
		case render::Order::Kind::Ambient_Light_Pop:
			assert(!ambient_lights.empty());
			ambient_lights.pop_back();
			break;
		default: break;
		}
	}

	auto& shader = asset::Store.get_shader(asset::Shader_Id::Light);
	shader.use();
	if (!ambient_lights.empty()) {
		auto& back = ambient_lights.back();
		shader.set_uniform("ambient_light", back.color / 255);
		shader.set_uniform("ambient_intensity", back.intensity);
	}
	shader.set_uniform("debug", (int)Environment.debug_framebuffer);
	shader.set_uniform("n_light_points", (int)point_light_idx);
	shader.set_uniform("buffer_albedo", 0);
	shader.set_uniform("buffer_normal", 1);
	shader.set_uniform("buffer_position", 2);

	g_buffer.render_quad();

	texture_target.set_active();
	hdr_buffer.set_active_texture();

	static float gamma{ .7f };
	static float exposure{ 1 };

	ImGui::Begin("Hdr");
	defer{ ImGui::End(); };

	ImGui::InputFloat("gamma", &gamma);
	ImGui::InputFloat("exposure", &exposure);

	auto& shader_hdr = asset::Store.get_shader(asset::Shader_Id::HDR);
	shader_hdr.use();
	shader_hdr.set_uniform("gamma", gamma);
	shader_hdr.set_uniform("exposure", exposure);
	shader_hdr.set_uniform("hdr_texture", 0);

	glViewport(0, 0, (GLsizei)Environment.window_width, (GLsizei)Environment.window_height);

	hdr_buffer.render_quad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	texture_target.render_quad();
	hdr_buffer.set_disable_texture();

	for (size_t i = 0; i < orders.late.size(); ++i) {
		auto& x = orders.late[i];

		switch (x.kind) {
		case render::Order::Kind::View_Pop: {
			assert(!view_stack.empty());
			view_stack.pop_back();
			if (!view_stack.empty()) render::current_view = view_stack.back();
			break;
		}
		case render::Order::Kind::View_Push: {
			view_stack.push_back(x.view);
			render::current_view = view_stack.back();
			break;
		}
		case render::Order::Kind::Sprite:
			render::immediate(x.sprite);
			break;
		case render::Order::Kind::Rectangle:
			render::immediate(x.rectangle);
			break;
		case render::Order::Kind::Circle:
			render::immediate(x.circle);
			break;
		case render::Order::Kind::Arrow:
			render::immediate(x.arrow);
			break;
		case render::Order::Kind::Line:
			render::immediate(x.line);
			break;
		default: assert("Logic error.");
		}
	}
}

bool init_gl_context() noexcept {
	EmscriptenWebGLContextAttributes attribs;
	emscripten_webgl_init_context_attributes(&attribs);

	attribs.majorVersion = 2;
	attribs.minorVersion = 0;

	auto context = emscripten_webgl_create_context(nullptr, &attribs);
	if (context <= 0) return false;
	auto result = emscripten_webgl_make_context_current(context);
	if (result != EMSCRIPTEN_RESULT_SUCCESS) return false;
	return result == EMSCRIPTEN_RESULT_SUCCESS;
}


void setup_controls_callback() noexcept {
	auto mouse = [](int event_type, const EmscriptenMouseEvent* event, void*) -> int {
		FunImGui::mouseCallback(event_type, event, nullptr);
		return 1;
	};
	auto wheel = [](int event_type, const EmscriptenWheelEvent* event, void*) -> int {
		FunImGui::wheelCallback(event_type, event, nullptr);
		return 1;
	};
	auto key = [](int event_type, const EmscriptenKeyboardEvent* event, void*) -> int {
		FunImGui::keyboardCallback(event_type, event, nullptr);
		return 1;
	};

	emscripten_set_mousemove_callback(nullptr, nullptr, false, mouse);
	emscripten_set_mousedown_callback(nullptr, nullptr, false, mouse);
	emscripten_set_mouseup_callback(nullptr, nullptr, false, mouse);

	emscripten_set_wheel_callback("canvas", nullptr, false, wheel);
	emscripten_set_wheel_callback("canvas", nullptr, false, wheel);

	//emscripten_set_keypress_callback(nullptr, nullptr, false, &FunImGui::keyboardCallback);
	emscripten_set_keydown_callback(nullptr, nullptr, false, key);
	emscripten_set_keyup_callback(nullptr, nullptr, false, key);
}

#endif

