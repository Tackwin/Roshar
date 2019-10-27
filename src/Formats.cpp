#include "Formats.hpp"

#include "OS/file.hpp"

std::optional<formats::png> formats::png_from_file(std::filesystem::path path) noexcept {
	auto opt_raw = file::read_whole_file(path);
	if (!opt_raw) return std::nullopt;
	auto& raw = *opt_raw;

	std::optional<formats::png> result = std::nullopt;
	size_t it = 0;

	if (raw.size() < 8) return std::nullopt;
	for (; it < 8; ++it) if (raw[it] != formats::png::magic[it]) return std::nullopt;

	#define SWAP_UINT32(x)\
(((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

	auto load_uint32 = [] (std::uint8_t* b) noexcept -> std::uint32_t {
		auto x = *reinterpret_cast<std::uint32_t*>(b);
		return SWAP_UINT32(x);
	};

	auto load_chunk = [&] noexcept {
		formats::png::Chunk c;
		
		c.length = load_uint32(&raw[it + 0]);
		c.type   = load_uint32(&raw[it + 4]);
		c.data   = &raw[it + 8];
		c.crc    = load_uint32(&raw[it + 8 + c.length]);

		return c;
	};



	return {};
}
