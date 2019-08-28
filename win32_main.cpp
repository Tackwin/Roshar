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

#include "Math/Vector.hpp"
#include "Graphic/Graphics.hpp"
#include "Graphic/OpenGL.hpp"

#include <imgui.h>
#include "Graphic/imgui_impl_win32.hpp"
#include "Graphic/imgui_impl_opengl3.hpp"

#include "Managers/InputsManager.hpp"
#include "Game.hpp"

static int attribs[] = {
	WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
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
		CS_CLASSDC,
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
	if (!GetWindowRect(window_handle, &window_rect)) {
		// >TODO error handling
		printf(get_last_error_message()->c_str());
		return 1;
	}
	Environment.window_width = (size_t)(window_rect.right - window_rect.left);
	Environment.window_height = (size_t)(window_rect.bottom - window_rect.top);


	auto gl_context = *create_gl_context(window_handle);
	defer{ destroy_gl_context(gl_context); };

	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageCallback((GLDEBUGPROCARB)opengl_debug, nullptr);
	}

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
		ImGui::End();

		ImGui::Begin("Perf");

		float avg = 0;
		for (auto x : last_dt) avg += x;
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
		orders.list.resize(0);

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
	glClearColor(0.6f, 0.3f, 0.4f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	std::vector<render::View_Info> view_stack;

	for (const auto& x : orders.list) {
		switch (x.kind) {
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
		case render::Order::Kind::View_Push: {
			auto y = x.view;
			view_stack.push_back(x.view);
			render::current_view = y;
			break;
		}
		case render::Order::Kind::View_Pop: {
			assert(!view_stack.empty());
			view_stack.pop_back();
			render::current_view = view_stack.back();
			break;
		}
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
		131185
	};

	constexpr GLenum To_Break_On[] = {
		1280, 1282, 1286
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
		//DebugBreak();
	}
}

#endif