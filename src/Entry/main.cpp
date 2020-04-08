#include <imgui.h>
#include <atomic>
#include <GL/glew.h>
#include <GL/wglew.h>

#include "Graphic/Graphics.hpp"
#include "Graphic/Graphics.hpp"
#include "Graphic/FrameBuffer.hpp"
#include "Graphic/UI/Kit.hpp"
#include "Managers/AssetsManager.hpp"

#include "Game.hpp"

#include "Profiler/Tracer.hpp"


constexpr Vector2u Gl_Buffer_Size = { 1920, 1080 };

void startup() noexcept;
void shutup() noexcept;
void post_char(std::uint32_t arg) noexcept;
void update_game(std::uint64_t dt) noexcept;
void render_game(render::Orders& orders) noexcept;
void render_orders(render::Orders& orders) noexcept;

void startup() noexcept {
	PROFILER_BEGIN_SEQ("monitor");
	asset::Store.monitor_path("assets/");
	PROFILER_SEQ("load_config");
	asset::Store.load_from_config("assets/config.json");
	PROFILER_SEQ("load_shaders");
	asset::Store.load_known_shaders();
	PROFILER_SEQ("load_textures");
	asset::Store.load_known_textures();
	PROFILER_SEQ("load_font");
	asset::Store.load_known_fonts();
	PROFILER_SEQ("init");
	static Game local_game;
	game = &local_game;
	game->load_start_config();
	PROFILER_END_SEQ();
}

void shutup() noexcept {
	PROFILER_BEGIN_SEQ("shutup");
	asset::Store.stop = true;
	PROFILER_SEQ("save_profile");
	asset::Store.save_profiles("assets/config.json", game->profiles);
	PROFILER_END_SEQ();
}

void post_char(std::uint32_t arg) noexcept {
	if (arg > 0xFF) {
		fprintf(stderr, "Fuck it just received a non ascii char not gonna handle it.\n");
		return;
	}

	IM::current_char = (char)arg;
}

void update_game(std::uint64_t dt) noexcept {
	Environment.window_ratio = Environment.window_height / (1.f * Environment.window_width);
	
	ImGui::Begin("Environment");
	int x = IM::controller_idx;
	ImGui::InputInt("Controller #", &x);
	IM::controller_idx = (size_t)std::clamp(x, 0, 4);
	ImGui::InputInt("Drag angle step", &Environment.drag_angle_step);
	ImGui::InputFloat("Gather speed", &Environment.gather_speed);
	ImGui::InputFloat("Gather step", &Environment.gather_step);
	ImGui::InputFloat("Drag", &Environment.drag);
	ImGui::InputFloat("Binding range", &Environment.binding_range);
	ImGui::InputFloat("Gravity", &Environment.gravity);
	ImGui::InputFloat("Dead velocity", &Environment.dead_velocity);
	ImGui::InputFloat("Speed up", &Environment.speed_up_step);
	ImGui::InputFloat("A", &Environment.a);
	ImGui::InputFloat("B", &Environment.b);
	ImGui::InputFloat("C", &Environment.c);
	ImGui::Checkbox("Debug Input", &Environment.debug_input);
	ImGui::Checkbox("Show Sprite", &Environment.show_sprite);
	ImGui::Checkbox("Show Camera Target", &Environment.show_camera_target);

	x = (int)Environment.debug_framebuffer;

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
			}
			return true;
		},
		nullptr,
		(int)Debug_Framebuffer::Count
	);
	Environment.debug_framebuffer = (Debug_Framebuffer)x;

	ImGui::End();

	ImGui::Begin("Update debug");
	game->update(dt);
	ImGui::End();
}

void render_game(render::Orders& orders) noexcept {
	game->render(orders);
	kit::render(orders);
	render_orders(orders);
	orders.clear();
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
		case render::Order::Kind::Text:
			render::immediate(x.text);
			break;
		default: assert("Logic error.");
		}
	}

	hdr_buffer.set_active();
	g_buffer.set_active_texture();
	glClearColor(0.6f, 0.3f, 0.4f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	std::vector<render::Ambient_Light> ambient_lights;
	ambient_lights.push_back({ .color = {1, 1, 1, 1}, .intensity = 1 });

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
		case render::Order::Kind::Text:{
			size_t idx = (size_t)&x.text.text[0];
			char* ptr = (char*)&orders.data[idx];
			x.text.text = ptr;
			render::immediate(x.text);
			break;
		}
		default: assert("Logic error.");
		}
	}
}