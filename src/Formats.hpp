#pragma once

#include <optional>
#include <filesystem>

namespace formats {
	struct png {
		static constexpr std::uint8_t magic[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

		struct Chunk {
			enum Type {
				IHDR = 'RDHI'
			};

			std::uint32_t length;
			std::uint32_t type;
			std::uint8_t* data;
			std::uint32_t crc;
        };
	};

	extern std::optional<png> png_from_file(std::filesystem::path path) noexcept;
}
