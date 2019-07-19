#pragma once

constexpr unsigned SEED = 0;
constexpr auto PI = 3.1415926;
constexpr auto LEVEL_PATH = "levels/";

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
	X(Win_File_Write);
	X(Win_File_Incomplete_Write);
#undef X
}

constexpr size_t operator""_id(const char* user, size_t size) {
	size_t seed = 0;
	for (size_t i = 0; i < size; ++i) seed = xstd::hash_combine(seed, (size_t)user[i]);
	return seed;
}

