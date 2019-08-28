#pragma once

#include <string>

#include "Math/Vector.hpp"

struct TextureInfo {
	size_t id = 0u;
	size_t width = 0u;
	size_t height = 0u;
};

class Texture {
public:
	Texture() noexcept;
	~Texture() noexcept;

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;

	bool load_file(const std::string& path);
	void create_rgb_null(Vector2u size) const;
	void create_depth_null(Vector2u size) const;

	void set_parameteri(int parameter, int value) const;
	void set_parameterfv(int parameter, float* value) const;

	void bind(size_t unit = 0u) const;

	size_t get_texture_id() const;

	Vector2u get_size() noexcept;
private:
	TextureInfo info;
};