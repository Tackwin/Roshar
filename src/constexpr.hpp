#pragma once

#include <cstdint>

constexpr unsigned SEED = 0;
constexpr auto PI = 3.1415926;
constexpr auto LEVEL_PATH = "assets/levels/";

constexpr std::uint64_t RECORD_LEVEL_TEST_ID = 1;

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


constexpr size_t operator""_id(const char* user, size_t size) {
	size_t seed = 0;
	for (size_t i = 0; i < size; ++i) seed = xstd::hash_combine(seed, (size_t)user[i]);
	return seed;
}

