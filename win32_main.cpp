#ifdef NATIVE

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <Windows.h>
#include <wingdi.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <optional>
#include <string>
#include <assert.h>
#include <io.h>
#include <fcntl.h>

#include <magic_enum.hpp>

#include "Math/Vector.hpp"
#include "Graphic/Graphics.hpp"
#include "Graphic/OpenGL.hpp"
#include "Graphic/FrameBuffer.hpp"

#include <imgui.h>
#include "Graphic/imgui_impl_win32.hpp"
#include "Graphic/imgui_impl_opengl3.hpp"

#include "Managers/InputsManager.hpp"
#include "Game.hpp"

constexpr Vector2u Gl_Buffer_Size = { 1920, 1080 };

static int attribs[] = {
#ifndef NDEBUG
	WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
	0
};

extern void update_game(std::uint64_t dt) noexcept;
extern void render_game(render::Orders& orders) noexcept;

void APIENTRY opengl_debug(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void*
) noexcept;

std::optional<std::string> get_last_error_message() noexcept;
std::optional<HGLRC> create_gl_context(HWND handle_window) noexcept;
void destroy_gl_context(HGLRC gl_context) noexcept;

IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
);

void render_orders(render::Orders& orders) noexcept;

LRESULT WINAPI window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	switch (msg) {
	case WM_MOUSEWHEEL:
		wheel_scroll = GET_WHEEL_DELTA_WPARAM(wParam) / 120.f;
		break;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}



#ifdef CONSOLE
int main(int, char**
#else
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
#endif
) {
	
	constexpr auto Class_Name = TEXT("Roshar Class");
	constexpr auto Window_Title = TEXT("Roshar");

	// Create application window
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC | CS_OWNDC,
		window_proc,
		0L,
		0L,
		GetModuleHandle(nullptr),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		Class_Name,
		nullptr
	};

	RegisterClassEx(&wc);
	defer{ UnregisterClass(Class_Name, wc.hInstance); };


	HWND window_handle = CreateWindow(
		Class_Name,
		Window_Title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1280,
		720,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	platform::handle_window = window_handle;

	RECT window_rect;
	if (!GetClientRect(window_handle, &window_rect)) {
		// >TODO error handling
		printf(get_last_error_message()->c_str());
		return 1;
	}
	Environment.window_width = (size_t)(window_rect.right - window_rect.left);
	Environment.window_height = (size_t)(window_rect.bottom - window_rect.top);


	auto gl_context = *create_gl_context(window_handle);
	defer{ destroy_gl_context(gl_context); };

#ifndef NDEBUG
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageCallback((GLDEBUGPROCARB)opengl_debug, nullptr);
	}
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	printf("Opengl ");
	printf((char*)glGetString(GL_VERSION));
	printf("\n");

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	const char* glsl_version = "#version 130";
	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplOpenGL3_Init(glsl_version);

	auto dc_window = GetDC(window_handle);
	if (!dc_window) {
		// >TODO error handling
		printf(get_last_error_message()->c_str());
		return 1;
	}

	platform::handle_dc_window = dc_window;

	asset::Store.monitor_path("shaders/");
	asset::Store.monitor_path("textures/");
	
	asset::Store.load_known_shaders();
	asset::Store.load_known_textures();

	wglSwapIntervalEXT(0);

	MSG msg{};

	render::Sprite_Info o;

	render::Orders orders;
	ShowWindow(window_handle, SW_SHOWDEFAULT);

	float max_dt = 0;
	size_t last_dt_count = 200;
	std::vector<float> last_dt;

	game = new Game;
	defer{ delete game; };

	auto last_time_frame = microseconds();
	while (msg.message != WM_QUIT) {
		std::uint64_t dt = microseconds() - last_time_frame;
		last_time_frame = microseconds();

		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		Main_Mutex.lock();
		defer{ Main_Mutex.unlock(); };
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Environment");
		ImGui::InputInt("Drag angle step", &Environment.drag_angle_step);
		ImGui::InputFloat("Gather speed", &Environment.gather_speed);
		ImGui::InputFloat("Gather step", &Environment.gather_step);
		ImGui::InputFloat("Drag", &Environment.drag);
		ImGui::InputFloat("Binding range", &Environment.binding_range);
		ImGui::InputFloat("Gravity", &Environment.gravity);
		ImGui::InputFloat("Dead velocity", &Environment.dead_velocity);
		ImGui::InputFloat("Speed up", &Environment.speed_up_step);
		int x = (int)Environment.debug_framebuffer;
		ImGui::ListBox(
			"Debug Framebuffer",
			&x,
			[](void*, int i, const char** out) {
				if (i >= (int)Debug_Framebuffer::Count) return false;
				*out = magic_enum::enum_name<Debug_Framebuffer>((Debug_Framebuffer)i).data();
				return true;
			},
			nullptr,
			(int)Debug_Framebuffer::Count
		);
		Environment.debug_framebuffer = (Debug_Framebuffer)x;

		ImGui::End();

		ImGui::Begin("Perf");

		float avg = 0;
		for (auto y : last_dt) avg += y;
		avg /= last_dt_count;

		ImGui::Text(
			"current dt: %llu ms, avg(%u): %llu ms, max: %llu",
			dt / 1000,
			last_dt_count,
			avg / 1000,
			max_dt / 1000
		);
		ImGui::Text("Fps: %d", (size_t)(1'000'000.0 / dt));

		ImGui::End();

		update_game(dt);
		render_game(orders);
		render_orders(orders);
		orders.clear();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SwapBuffers(dc_window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;
}

void render_orders(render::Orders& orders) noexcept {
	static Texture_Buffer texture_target{ Gl_Buffer_Size };
	static G_Buffer   g_buffer{ Gl_Buffer_Size };
	static HDR_Buffer hdr_buffer{ Gl_Buffer_Size };
	std::vector<render::View_Info> view_stack;

	glViewport(0, 0, Gl_Buffer_Size.x, Gl_Buffer_Size.y);

	g_buffer.set_active();
	glClearColor(0, 0, 0, 1.f);
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
	ambient_lights.push_back({ .color = {1, 1, 1, 1}, .intensity = {1} });

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
		}
	}

	auto& shader = asset::Store.get_shader(asset::Known_Shaders::Light);
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

	auto& shader_hdr = asset::Store.get_shader(asset::Known_Shaders::HDR);
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

std::optional<std::string> get_last_error_message() noexcept {
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::nullopt; //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	auto flags = FORMAT_MESSAGE_ALLOCATE_BUFFER;
	flags |= FORMAT_MESSAGE_FROM_SYSTEM;
	flags |= FORMAT_MESSAGE_IGNORE_INSERTS;
	size_t size = FormatMessageA(
		flags, nullptr, errorMessageID, 0, (LPSTR)& messageBuffer, 0, nullptr
	);

	std::string message(messageBuffer, size);

	LocalFree(messageBuffer);
	return message;
}


std::optional<HGLRC> create_gl_context(HWND handle_window) noexcept {
	auto dc = GetDC(handle_window);
	if (!dc) {
		// >TODO error handling
		printf(get_last_error_message()->c_str());
		return std::nullopt;
	}
	defer{ ReleaseDC(handle_window, dc); };

	PIXELFORMATDESCRIPTOR pixel_format{};
	pixel_format.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixel_format.nVersion = 1;
	pixel_format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pixel_format.cColorBits = 32;
	pixel_format.cAlphaBits = 8;
	pixel_format.iLayerType = PFD_MAIN_PLANE;

	auto suggested_pixel_format = ChoosePixelFormat(dc, &pixel_format);
	if (!suggested_pixel_format) {
		// >TODO error handling
		printf(get_last_error_message()->c_str());
		return std::nullopt;
	}
	auto result = DescribePixelFormat(
		dc, suggested_pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format
	);
	if (!result) {
		// >TODO error handling
		printf(get_last_error_message()->c_str());
		return std::nullopt;
	}

	if (!SetPixelFormat(dc, suggested_pixel_format, &pixel_format)) {
		// >TODO error handling
		printf(get_last_error_message()->c_str());
		return std::nullopt;
	}


	auto gl_context = wglCreateContext(dc);
	if (!gl_context) {
		// >TODO error handling
		printf(get_last_error_message()->c_str());
		return std::nullopt;
	}

	if (!wglMakeCurrent(dc, gl_context)) {
		wglDeleteContext(gl_context);

		printf(get_last_error_message()->c_str());
		return std::nullopt;
	}

	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		printf("Can't init glew\n");
		return gl_context;
	}

	auto gl = wglCreateContextAttribsARB(dc, nullptr, attribs);
	if (!gl) {
		auto err = glGetError();
		printf(std::to_string(err).c_str());

		return gl_context;
	}
	platform::main_opengl_context = gl;

	if (!wglMakeCurrent(dc, gl)) {
		wglDeleteContext(gl);
		return gl_context;
	}


	wglDeleteContext(gl_context);
	return (HGLRC)platform::main_opengl_context;
}
void destroy_gl_context(HGLRC gl_context) noexcept {
	// >TODO error handling
	if (!wglDeleteContext(gl_context)) printf(get_last_error_message()->c_str());
}


const char* debug_source_str(GLenum source) {
	static const char* sources[] = {
	  "API",   "Window System", "Shader Compiler", "Third Party", "Application",
	  "Other", "Unknown"
	};
	int str_idx = std::min(
		source - GL_DEBUG_SOURCE_API, sizeof(sources) / sizeof(const char*) - 1
	);
	return sources[str_idx];
}

const char* debug_type_str(GLenum type) {
	static const char* types[] = {
	  "Error",       "Deprecated Behavior", "Undefined Behavior", "Portability",
	  "Performance", "Other",               "Unknown"
	};

	int str_idx = std::min(type - GL_DEBUG_TYPE_ERROR, sizeof(types) / sizeof(const char*) - 1);
	return types[str_idx];
}

const char* debug_severity_str(GLenum severity) {
	static const char* severities[] = {
	  "High", "Medium", "Low", "Unknown"
	};

	int str_idx = std::min(
		severity - GL_DEBUG_SEVERITY_HIGH, sizeof(severities) / sizeof(const char*) - 1
	);
	return severities[str_idx];
}

void APIENTRY opengl_debug(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei,
	const GLchar* message,
	const void*
) noexcept {
	constexpr GLenum To_Ignore[] = {
		131185,
		131204
	};

	constexpr GLenum To_Break_On[] = {
		1280, 1281, 1282, 1286
	};

	if (std::find(BEG_END(To_Ignore), id) != std::end(To_Ignore)) return;

	printf("OpenGL Error:\n");
	printf("=============\n");
	printf(" Object ID: ");
	printf(std::to_string(id).c_str());
	printf("\n Severity:  ");
	printf(debug_severity_str(severity));
	printf("\n Type:      ");
	printf(debug_type_str(type));
	printf("\n Source:    ");
	printf(debug_source_str(source));
	printf("\n Message:   ");
	printf(message);
	printf("\n\n");

	if (std::find(BEG_END(To_Break_On), id) != std::end(To_Break_On)) {
		DebugBreak();
	}
}

#endif