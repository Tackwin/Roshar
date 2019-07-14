#pragma once

#include <thread>
#include <optional>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <filesystem>

namespace file {
	[[nodiscard]] extern xstd::std_expected<std::string>
		read_whole_text(const std::filesystem::path& path) noexcept;
	[[nodiscard]] extern xstd::std_expected<std::vector<char>>
		read_whole_file(const std::filesystem::path& path) noexcept;

	extern bool
		overwrite_file(const std::filesystem::path& path, std::string_view str) noexcept;
	struct OpenFileOpts {
		void* owner{ nullptr };

		std::unordered_map<std::string, std::vector<std::string>> ext_filters;
		std::filesystem::path filepath;
		std::filesystem::path filename;
		std::string dialog_title{ NULL };
		std::string default_ext{ NULL };

		bool allow_multiple{ false };
		bool prompt_for_create{ false };
		bool allow_redirect_link{ false };
	};
	struct OpenFileResult {
		bool succeded{ false };

		unsigned long error_code{ 0 };

		std::filesystem::path filepath;
		std::filesystem::path filename;
	};

	extern void open_file_async(
		std::function<void(OpenFileResult)>&& callback, OpenFileOpts opts = OpenFileOpts{}
	) noexcept;
	extern void open_dir_async(
		std::function<void(std::optional<std::filesystem::path>)>&& callback
	) noexcept;
	extern std::optional<std::filesystem::path> open_dir() noexcept;
	extern OpenFileResult open_file(OpenFileOpts opts = OpenFileOpts{}) noexcept;

}
