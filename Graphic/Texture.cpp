#include "Graphic/Texture.hpp"
#include <GL/glew.h>

// >STB >SFML >SEE:
// SFML already define this in its static lib don't forget to remove it when we do the transition.
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

Texture::Texture() noexcept {
	glGenTextures(1, &info.id);
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
Texture::~Texture() noexcept {
	glDeleteTextures(1, &info.id);
}

Texture::Texture(Texture&& that) noexcept {
	if (&that == this) return;

	info = that.info;
	that.info = {};
}
Texture& Texture::operator=(Texture&& that) noexcept {
	if (&that == this) return *this;

	this->~Texture();
	info = that.info;
	that.info = {};
	return *this;
}

bool Texture::load_file(const std::string& path) {
	auto comp = 0;

	stbi_set_flip_vertically_on_load(true);

	int w = (int)info.width;
	int h = (int)info.height;
	std::uint8_t* data = stbi_load(
		path.c_str(), &w, &h, &comp, STBI_rgb_alpha
	);
	defer{ stbi_image_free(data); };
	info.width = w;
	info.height = h;

	if (!data) return false;

	bind();

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		info.width, 
		info.height, 
		0, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		data
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}
void Texture::create_rgb_null(Vector2u size) const {
	bind();
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		size.x,
		size.y,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		NULL
	);
	glGenerateMipmap(GL_TEXTURE_2D);
}
void Texture::create_depth_null(Vector2u size) const {
	bind();
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH24_STENCIL8,
		size.x,
		size.y,
		0,
		GL_DEPTH_STENCIL,
		GL_UNSIGNED_BYTE,
		NULL
	);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::set_parameteri(int parameter, int value) const {
	bind();
	glTexParameteri(GL_TEXTURE_2D, parameter, value);
}
void Texture::set_parameterfv(int parameter, float* value) const {
	bind();
	glTexParameterfv(GL_TEXTURE_2D, parameter, value);
}

void Texture::bind(size_t unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, info.id);
}

size_t Texture::get_texture_id() const {
	return info.id;
}

Vector2u Texture::get_size() noexcept {
	return { info.width, info.height };
}
