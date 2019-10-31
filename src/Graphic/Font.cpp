#include "Font.hpp"

#include <cassert>

#include "dyn_struct.hpp"

void from_dyn_struct(const dyn_struct& str, Font::Font_Info& font) noexcept {
	font.char_size = (size_t)str["size"];
	font.texture_path = (std::string)str["texture"];
	font.family = (std::string)str["Consolas"];
	font.height = (size_t)str["height"];
	font.style = (Font::Font_Info::Style)(int)str["style"];
	for (auto& x : iterate_array(str["Char"])) {
		Font::Font_Info::Char c;
		c.width = (size_t)x["width"];
		c.code = (std::uint8_t)(int)x["width"];
		c.rect = (Rectangleu)x["recet"];
		c.offset = (Vector2i)x["offset"];
		font.chars.push_back(c);
	}
}

void to_dyn_struct(dyn_struct& str, const Font::Font_Info& font) noexcept {
	assert(false);
}