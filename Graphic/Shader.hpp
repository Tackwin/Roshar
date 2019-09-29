#pragma once

#include <string>

#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"
#include "Math/Matrix.hpp"

struct ShaderInfo {
	std::uint32_t vertexId = 0;
	std::uint32_t programId = 0;
	std::uint32_t fragmentId = 0;

	bool vertex_compiled{ false };
	bool vertex_linked{ false };

	bool fragment_compiled{ false };
	bool fragment_linked{ false };
};

class Shader {
public:
	static std::optional<Shader> create_shader(
		std::filesystem::path vertex, std::filesystem::path fragment
	) noexcept; 

public:
	Shader() noexcept;
	~Shader() noexcept;
	
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& that) noexcept;
	Shader& operator=(Shader&& that) noexcept;

	void use() const noexcept;

	void set_window_size(Vector2u size) noexcept;
	void set_view(Rectanglef view) noexcept;
	void set_primary_color(Vector4d color) noexcept;
	void set_rotation(float rotation) noexcept;
	void set_origin(Vector2f origin) noexcept;
	void set_position(Vector2f pos) noexcept;
	void set_size(Vector2f size) noexcept;
	void set_texture(std::uint32_t id) noexcept;

	void set_use_texture(bool x) noexcept;

	bool load_vertex(std::filesystem::path path) noexcept;
	bool load_fragment(std::filesystem::path path) noexcept;
	bool build_shaders() noexcept;


	void set_uniform(const std::string& name, Rectanglef x) noexcept;
	void set_uniform(const std::string& name, Vector2f x) noexcept;
	void set_uniform(const std::string& name, Vector4u x) noexcept;
	void set_uniform(const std::string& name, Vector4d x) noexcept;
	void set_uniform(const std::string& name, float x) noexcept;
	void set_uniform(const std::string& name, int x) noexcept;

private:
	std::unordered_map<std::string, std::uint32_t> cache_loc;

	std::optional<std::string> check_shader_error(std::uint32_t shader) noexcept;
	std::optional<std::string> check_program_error(std::uint32_t program) noexcept;

	ShaderInfo info;
};

