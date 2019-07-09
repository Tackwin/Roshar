#pragma once

constexpr unsigned SEED = 0;

namespace Error {
#define X(x) constexpr auto x = #x
	X(Shader_Fragment_Load);
	X(Shader_Vertex_Load);
	X(Win_Create_File);
	X(Win_File_Size);
	X(Win_File_Read);
#undef X
}
