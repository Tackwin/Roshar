#pragma once

constexpr unsigned SEED = 0;
constexpr auto PI = 3.1415926;

namespace math {
	template<typename S>
	constexpr S scale_zoom(S x) noexcept {
		constexpr auto max = [](auto a, auto b) noexcept { return a > b ? a : b; };
		constexpr auto min = [](auto a, auto b) noexcept { return a < b ? a : b; };
		constexpr S one = static_cast<S>(1);
		constexpr S zero = static_cast<S>(0);

		return max(zero, x - one) - one / (min(x - one, zero) - one);
	}
}

namespace Error {
#define X(x) constexpr auto x = #x
	X(Shader_Fragment_Load);
	X(Shader_Vertex_Load);
	X(Win_Create_File);
	X(Win_File_Size);
	X(Win_File_Read);
#undef X
}
