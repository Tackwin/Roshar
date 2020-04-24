
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

io::Keyboard_State emscripten_keyboard_state{};
io::Controller_State emscripten_controller_state{};
Vector2f emscripten_mouse_pos{};

constexpr Vector2u Gl_Buffer_Size = { 1280, 720 };

bool init_gl_context() noexcept;
void setup_controls_callback() noexcept;
void loop() noexcept;
void render_orders(render::Orders& orders) noexcept;

extern void startup() noexcept;
extern void shutup() noexcept;
extern void post_char(std::uint32_t codepoint) noexcept;
extern void update_game(std::uint64_t dt) noexcept;
extern void render_game(render::Orders& orders) noexcept;
extern void render_orders(render::Orders& orders) noexcept;

int main() {
	if (!init_gl_context()) {
		printf("Error on context init.\n");
		return 1;
	}

	setup_controls_callback();

	FunImGui::init();
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	asset::Store.monitor_path("shaders/");
	asset::Store.monitor_path("textures/");

	asset::Store.load_from_config("assets/config.json");
	asset::Store.load_known_shaders();
	asset::Store.load_known_textures();

	Environment.window_width = 1280;
	Environment.window_height = 720;

	startup();

	emscripten_set_main_loop(loop, 0, 1);

	shutup();

	return 0;
}

void loop() noexcept{
	static render::Orders orders;
	static auto last_time_frame = microseconds();
	std::uint64_t dt = microseconds() - last_time_frame;
	last_time_frame = microseconds();

	Main_Mutex.lock();
	defer{ Main_Mutex.unlock(); };


	FunImGui::BeginFrame();

	update_game(dt);

	auto t_start = seconds();
	render_game(orders);
	auto cpu_render_time = seconds() - t_start;

	ImGui::Render();
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
		emscripten_keyboard_state.keys[io::Keyboard_State::Max_Key - 1] = event->buttons & 1;
		emscripten_keyboard_state.keys[io::Keyboard_State::Max_Key - 2] = event->buttons & 2;
		emscripten_keyboard_state.keys[io::Keyboard_State::Max_Key - 3] = event->buttons & 4;
		return 1;
	};
	auto wheel = [](int event_type, const EmscriptenWheelEvent* event, void*) -> int {
		FunImGui::wheelCallback(event_type, event, nullptr);
		if (event->deltaY > 0)
			wheel_scroll = -1.f / 5.f;
		else if (event->deltaY < 0)
			wheel_scroll = 1.f / 5.f;
		return 1;
	};
	auto key = [](int event_type, const EmscriptenKeyboardEvent* event, void*) -> int {
		FunImGui::keyboardCallback(event_type, event, nullptr);
		
		switch (event_type) {
		case EMSCRIPTEN_EVENT_KEYDOWN: {
			emscripten_keyboard_state.keys[event->which] = 1;
			break;
		}
		case EMSCRIPTEN_EVENT_KEYUP: {
			emscripten_keyboard_state.keys[event->which] = 0;
			break;
		}
		}
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

