#include "Formats.hpp"

#include "OS/file.hpp"

#include <assert.h>

using namespace formats;
#define CHECK(x) if (!(x)) return std::nullopt;


#define SWAP_UINT32(x)\
(((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

auto load_uint32 = [] (uint8_t* b) noexcept -> std::uint32_t {
	auto x = *reinterpret_cast<uint32_t*>(b);
	return SWAP_UINT32(x);
};
auto load_uint08 = [] (uint8_t* b) noexcept -> std::uint32_t {
	auto x = *reinterpret_cast<uint8_t*>(b);
	return x;
};

std::vector<uint8_t> formats::zlib_decode(uint8_t* data, uint32_t size) noexcept {
	uint8_t CMF = data[0];
	uint8_t FLG = data[1];

	uint8_t CM = CMF & 0b0000'1111;
	uint8_t CINFO  = (CMF & 0b1111'1111) >> 4;

	uint8_t FCHECK =  FLG & 0b0001'1111;
	uint8_t FDICT  = (FLG & 0b0010'0000) > 0;
	uint8_t FLEVEL = (FLG & 0b1100'0000) >> 6;
	return {};
}


bool png_t::IHDR::check_colour_type_bit_depth_compatibility() const noexcept {
	switch (colour_type) {
		// Each pixel is a greyscale sample.
		case 0: return
			compression_method == 1 ||
			compression_method == 2 ||
			compression_method == 4 ||
			compression_method == 8 ||
			compression_method == 16;
		// Each pixel is an R,G,B triple.
		case 2: return
			compression_method == 8 ||
			compression_method == 16;
		// Each pixel is a palette index; a PLTE chunk shall appear.
		case 3: return
			compression_method == 1 ||
			compression_method == 2 ||
			compression_method == 4 ||
			compression_method == 8;
		// Each pixel is a greyscale sample followed by an alpha sample.
		case 4: return
			compression_method == 8 ||
			compression_method == 16;
		// Each pixel is an R,G,B triple followed by an alpha sample.
		case 6: return
			compression_method == 8 ||
			compression_method == 16;
		default: return false;
	}
}


std::optional<png_t::IHDR> png_t::IHDR::decode(uint8_t* data, uint32_t size) noexcept{
	IHDR result;

	CHECK(size == 13);

	result.width              = load_uint32(&data[ 0]);
	result.height             = load_uint32(&data[ 4]);
	result.bit_depth          = load_uint08(&data[ 8]);
	result.colour_type        = load_uint08(&data[ 9]);
	result.compression_method = load_uint08(&data[10]);
	result.filter_method      = load_uint08(&data[11]);
	result.interlace_method   = load_uint08(&data[12]);

	CHECK(result.check_colour_type_bit_depth_compatibility());

	return result;
}

std::optional<png_t::PLTE> png_t::PLTE::decode(uint8_t* data, uint32_t size) noexcept{
	PLTE result;

	CHECK((size % 3) == 0);

	for (size_t i = 0; i < size; i += 3) {
		result.red  .push_back(data[i + 0]);
		result.green.push_back(data[i + 1]);
		result.blue .push_back(data[i + 2]);
	}

	return result;
}

std::optional<png_t::IDAT> png_t::IDAT::decode(uint8_t* data, uint32_t size) noexcept {
	IDAT result;
	result.data = data;
	result.size = size;
	return result;
}


std::optional<png_t> formats::png_from_file(std::filesystem::path path) noexcept {
	auto opt_raw = file::read_whole_file(path);
	if (!opt_raw) return std::nullopt;
	auto& raw = *opt_raw;

	std::optional<png_t> result = std::nullopt;
	size_t it = 0;

	CHECK(raw.size() >= 8);
	for (; it < 8; ++it) CHECK(raw[it] == png_t::magic[it]);

	auto load_chunk = [&] () noexcept {
		png_t::Chunk c;
		
		c.length = load_uint32(&raw[it + 0]);
		c.type   = load_uint32(&raw[it + 4]);
		c.data   = &raw[it + 8];
		c.crc    = load_uint32(&raw[it + 8 + c.length]);

		it += 8 + c.length;
		return c;
	};

	png_t png;
	png.chunks.push_back(load_chunk());
	CHECK(png.chunks.front().type == png_t::Chunk::Type::IHDR);
	png.chunks.push_back(load_chunk());
	CHECK(
		png.chunks.front().type == png_t::Chunk::Type::PLTE ||
		png.chunks.front().type == png_t::Chunk::Type::IEND ||
		png.chunks.front().type == png_t::Chunk::Type::IDAT
	);
	while(it < raw.size()) {
		png.chunks.push_back(load_chunk());
		CHECK(png.chunks.back().type != png_t::Chunk::Type::IHDR);
		CHECK(png.chunks.back().type != png_t::Chunk::Type::PLTE);
		CHECK(png.chunks.back().type != png_t::Chunk::Type::IEND);
	}
	CHECK(png.chunks.back().type == png_t::Chunk::Type::IEND);

	return png;
}
std::optional<png_image> formats::image_from_file(std::filesystem::path path) noexcept {
	auto opt = formats::png_from_file(path);
	CHECK(opt);
	return image_from_png(*opt);
}
std::optional<png_image> formats::image_from_png(const png_t& png) noexcept {
	return {};
}
