#pragma once
#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"

#include "Graphics.hpp"

// >Note: Tackwin
// Every time i do a class wrapper around an OpenGl concept i try to have const mean doesn't
// change the OpenGL state. Meaning technically most of the method could be const as they don't
// mutate any members but they do alters the opengl state.
struct G_Buffer {
	G_Buffer(G_Buffer&) = delete;
	G_Buffer& operator=(G_Buffer&) = delete;
	G_Buffer(G_Buffer&&) = default;
	G_Buffer& operator=(G_Buffer&&) = default;

	G_Buffer(Vector2u size) noexcept;
	~G_Buffer() noexcept;

	void set_active() noexcept;
	void set_active_texture() noexcept;
	void set_disable_texture() noexcept;

	void render_quad() noexcept;
	void copy_depth_to(uint32_t id = 0) noexcept;
	void copy_depth_to(uint32_t id, Rectanglef viewport) noexcept;

	void clear(Vector4d color) noexcept;
private:
	Vector2u size;

	uint32_t g_buffer{ 0 };
	uint32_t pos_buffer{ 0 };
	uint32_t normal_buffer{ 0 };
	uint32_t albedo_buffer{ 0 };
	uint32_t depth_rbo{ 0 };
	uint32_t quad_VAO{ 0 };
	uint32_t quad_VBO{ 0 };
};

struct HDR_Buffer {
	HDR_Buffer(HDR_Buffer&) = delete;
	HDR_Buffer& operator=(HDR_Buffer&) = delete;
	HDR_Buffer(HDR_Buffer&&) = default;
	HDR_Buffer& operator=(HDR_Buffer&&) = delete;

	HDR_Buffer(Vector2u size) noexcept;
	~HDR_Buffer() noexcept;

	Vector2u get_size() const noexcept;

	void set_active() noexcept;
	void set_active_texture() noexcept;
	void set_disable_texture() noexcept;

	void render_quad() noexcept;

	uint32_t get_depth_id() const noexcept;
private:
	Vector2u size;

	uint32_t hdr_buffer{ 0 };
	uint32_t rbo_buffer{ 0 };
	uint32_t color_buffer{ 0 };
	uint32_t quad_VAO{ 0 };
	uint32_t quad_VBO{ 0 };
};

struct Texture_Buffer {
	Texture_Buffer(Texture_Buffer&) = delete;
	Texture_Buffer& operator=(Texture_Buffer&) = delete;
	Texture_Buffer(Texture_Buffer&&) = default;
	Texture_Buffer& operator=(Texture_Buffer&&) = default;

	Texture_Buffer(Vector2u size) noexcept;
	~Texture_Buffer() noexcept;

	uint32_t get_frame_buffer_id() const noexcept;

	const Texture& get_texture() const noexcept;

	void set_active() noexcept;
	void set_active_texture() noexcept;

	void render_quad() noexcept;

	void clear(Vector4f color) noexcept;

private:
	uint32_t frame_buffer{ 0 };
	uint32_t rbo_buffer{ 0 };
	uint32_t quad_VAO{ 0 };
	uint32_t quad_VBO{ 0 };
	Texture texture;
};

struct SSAO_Buffer {
	SSAO_Buffer(SSAO_Buffer&) = delete;
	SSAO_Buffer& operator=(SSAO_Buffer&) = delete;
	SSAO_Buffer(SSAO_Buffer&&) = default;
	SSAO_Buffer& operator=(SSAO_Buffer&&) = default;

	SSAO_Buffer(Vector2u size) noexcept;
	~SSAO_Buffer() noexcept;

	void set_active_ssao() noexcept;
	void set_active_blur() noexcept;

	void set_active_texture_for_blur() noexcept;
	void set_active_texture(size_t n) noexcept;
	void render_quad() noexcept;

private:

	uint32_t quad_VAO{ 0 };
	uint32_t quad_VBO{ 0 };
	uint32_t ssao_buffer{ 0 };
	uint32_t ssao_blur_buffer{ 0 };
	uint32_t color_buffer{ 0 };
	uint32_t blur_buffer{ 0 };
};
