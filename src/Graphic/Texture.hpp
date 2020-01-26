#pragma once

#include <string>

#include "Math/Vector.hpp"

struct TextureInfo {
	std::uint32_t id = 0u;
	std::uint32_t width = 0u;
	std::uint32_t height = 0u;
};

class Texture {
public:
	enum class Filter {
		Linear = 0,
		Nearest,
		Count
	};

	Texture() noexcept;
	~Texture() noexcept;

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;

	bool load_file(std::filesystem::path path);
	void create_rgb_null(Vector2u size) const;
	void create_depth_null(Vector2u size) const;

	void set_parameteri(int parameter, int value) const;
	void set_parameterfv(int parameter, float* value) const;

	void set_resize_filter(Filter filter) noexcept;

	void bind(size_t unit = 0u) const;

	std::uint32_t get_texture_id() const;

	Vector2u get_size() noexcept;
private:
	TextureInfo info;
};