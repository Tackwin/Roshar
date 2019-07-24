#include "Time.hpp"

#include <chrono>

[[nodiscard]] double seconds() noexcept {
	return nanoseconds() / 1'000'000'000.0;
}

[[nodiscard]] std::uint64_t nanoseconds() noexcept {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}
[[nodiscard]] std::uint64_t microseconds() noexcept {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

