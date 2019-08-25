#pragma once

#include <string>

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

struct ShaderInfo {
	size_t vertexId = 0;
	size_t programId = 0;
	size_t fragmentId = 0;

	std::string vertexSource = "";
	std::string fragmentSource = "";
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

	void set_primary_color(Vector4d color) noexcept;
	void set_rotation(float rotation) noexcept;
	void set_origin(Vector2f origin) noexcept;
	void set_position(Vector2f pos) noexcept;
	void set_size(Vector2f size) noexcept;
	void set_texture(size_t id) noexcept;

private:
	bool load_vertex(std::filesystem::path path) noexcept;
	bool load_fragment(std::filesystem::path path) noexcept;
	bool build_shaders() noexcept;

	std::optional<std::string> check_shader_error(size_t shader) noexcept;
	std::optional<std::string> check_program_error(size_t program) noexcept;

	ShaderInfo info;
};

