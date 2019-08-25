#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <Windows.h>
#include <wingdi.h>
#include <GL/glew.h>
#include <optional>
#include <string>

#include "Math/Vector.hpp"

std::optional<std::string> get_last_error_message() noexcept;
std::optional<HGLRC> create_gl_context(HWND handle_window) noexcept;
void destroy_gl_context(HGLRC gl_context) noexcept;

LRESULT WINAPI window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
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

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
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
		0,
		0,
		1280,
		720,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	RECT window_rect;
	if (!GetWindowRect(window_handle, &window_rect)) {
		// >TODO error handling
		OutputDebugStringA(get_last_error_message()->c_str());
		return 1;
	}
	Environment.window_width = (size_t)(window_rect.right - window_rect.left);
	Environment.window_height = (size_t)(window_rect.bottom - window_rect.top);


	auto gl_context = *create_gl_context(window_handle);
	defer{ destroy_gl_context(gl_context); };

	if (glewInit() != GLEW_OK) {
		OutputDebugString("Can't init glew\n");
		return 1;
	}

	OutputDebugString("Opengl ");
	OutputDebugString((char*)glGetString(GL_VERSION));
	OutputDebugString("\n");

	auto dc_window = GetDC(window_handle);
	if (!dc_window) {
		// >TODO error handling
		OutputDebugStringA(get_last_error_message()->c_str());
		return 1;
	}


	ShowWindow(window_handle, SW_SHOWDEFAULT);

	MSG msg{};
	auto last_time_frame = seconds();
	while (msg.message != WM_QUIT) {
		double dt = seconds() - last_time_frame;
		last_time_frame = seconds();

		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}



		SwapBuffers(dc_window);
	}

	return 0;
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
		OutputDebugStringA(get_last_error_message()->c_str());
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
		OutputDebugStringA(get_last_error_message()->c_str());
		return std::nullopt;
	}
	auto result = DescribePixelFormat(
		dc, suggested_pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format
	);
	if (!result) {
		// >TODO error handling
		OutputDebugStringA(get_last_error_message()->c_str());
		return std::nullopt;
	}

	if (!SetPixelFormat(dc, suggested_pixel_format, &pixel_format)) {
		// >TODO error handling
		OutputDebugStringA(get_last_error_message()->c_str());
		return std::nullopt;
	}


	auto gl_context = wglCreateContext(dc);
	if (!gl_context) {
		// >TODO error handling
		OutputDebugStringA(get_last_error_message()->c_str());
		return std::nullopt;
	}

	if (!wglMakeCurrent(dc, gl_context)) {
		// >TODO error handling
		if (!wglDeleteContext(gl_context)) OutputDebugStringA(get_last_error_message()->c_str());

		OutputDebugStringA(get_last_error_message()->c_str());
		return std::nullopt;
	}
	return gl_context;
}
void destroy_gl_context(HGLRC gl_context) noexcept {
	// >TODO error handling
	if (!wglDeleteContext(gl_context)) OutputDebugStringA(get_last_error_message()->c_str());
}