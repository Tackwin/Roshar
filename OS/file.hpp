#pragma once
#include <filesystem>
#include "xstd.hpp"

namespace file {
	[[nodiscard]] extern xstd::std_expected<std::string> read_whole_text(const std::filesystem::path& path) noexcept;
}
