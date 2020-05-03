#pragma once

#include <vector>
#include <optional>
#include <filesystem>

namespace formats {
	extern std::vector<uint8_t> zlib_decode(uint8_t* data, uint32_t size) noexcept;

	struct png_t {
		static constexpr uint8_t magic[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

		struct Chunk {
			enum Type {
				IHDR = 'RDHI',
				PLTE = 'ETLP',
				IDAT = 'TADI',
				IEND = 'DNEI'
			};

			uint32_t length;
			uint32_t type;
			uint8_t* data;
			uint32_t crc;
		};

		struct IHDR {
			uint32_t width;
			uint32_t height;
			uint8_t bit_depth;
			uint8_t colour_type;
			uint8_t compression_method;
			uint8_t filter_method;
			uint8_t interlace_method;

			static std::optional<IHDR> decode(uint8_t* data, uint32_t size) noexcept;
			bool check_colour_type_bit_depth_compatibility() const noexcept;
		};

		struct PLTE {
			std::vector<uint8_t> red;
			std::vector<uint8_t> green;
			std::vector<uint8_t> blue;

			static std::optional<PLTE> decode(uint8_t* data, uint32_t size) noexcept;
		};

		struct IDAT {
			uint8_t* data;
			uint32_t size;
			static std::optional<IDAT> decode(uint8_t* data, uint32_t size) noexcept;
		};

		struct IEND {
			static std::optional<IEND> decode(uint8_t*, uint32_t) noexcept { return IEND{}; }
		};

		std::vector<Chunk> chunks;
	};

	struct png_image {
		std::vector<uint8_t> data;
		size_t width;
		size_t height;
		uint8_t bit_depth;
		uint8_t colour_type;
	};

	extern std::optional<png_t> png_from_file(std::filesystem::path path) noexcept;
	extern std::optional<png_image> image_from_file(std::filesystem::path path) noexcept;
	extern std::optional<png_image> image_from_png(const png_t& png) noexcept;
}
