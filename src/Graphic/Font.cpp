#include "Font.hpp"

#include <cassert>

#include "dyn_struct.hpp"

std::optional<Font::Font_Info::Char> Font::Font_Info::map(std::uint8_t code) const noexcept {
	for (auto& x : chars) if (x.code == code) return x;
	return std::nullopt;
}


void from_dyn_struct(const dyn_struct& str, Font::Font_Info& font) noexcept {
	font.char_size = (size_t)str["size"];
	font.texture_path = (std::string)str["texture"];
	font.family = (std::string)str["family"]; 
	font.height = (size_t)str["height"];
	
	if (auto e = (std::string)str["style"]; e == "Regular"){
		font.style = Font::Font_Info::Style::Regular;
	} else {
		assert(false);
	}

	for (auto& x : iterate_array(str["Char"])) {
		auto str = (std::string)x["code"];
		
		Font::Font_Info::Char c;
		c.width = (size_t)x["width"];
		c.code = (std::uint8_t)str[0];
		c.rect = (Rectangleu)x["rect"];
		c.offset = (Vector2i)x["offset"];
		font.chars.push_back(c);
	}
}

void to_dyn_struct(dyn_struct& str, const Font::Font_Info& font) noexcept {
	assert(false);
}