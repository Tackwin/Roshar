#pragma once

#include <GL/wglew.h>

namespace wgl {
	typedef HGLRC WINAPI Create_Context_Attribs_ARB(HDC, HGLRC, const int*);
	typedef HGLRC WINAPI Make_Current_ARB(HDC, HGLRC, const int*);

	inline static Create_Context_Attribs_ARB* create_context_arb{ nullptr };
}