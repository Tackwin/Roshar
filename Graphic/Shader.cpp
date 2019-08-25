#include <iostream>
#include <sstream>
#include <fstream>

#include <GL/glew.h>

#include "Graphic/Shader.hpp"
#include "OS/file.hpp"

std::optional<Shader> Shader::create_shader(
	std::filesystem::path vertex, std::filesystem::path fragment
) noexcept {
	Shader s;
	if (!s.load_fragment(fragment)) return std::nullopt;
	if (!s.load_vertex(vertex)) return std::nullopt;
	if (!s.build_shaders()) return std::nullopt;

	return s;
}

Shader::Shader() noexcept {
	info.vertexId = glCreateShader(GL_VERTEX_SHADER);
	info.fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
}

Shader::~Shader() noexcept {
	if (info.vertexId) glDeleteShader(info.vertexId);
	if (info.fragmentId) glDeleteShader(info.fragmentId);
	glDeleteProgram(info.programId);
}

Shader::Shader(Shader&& that) noexcept {
	if (&that == this) return;

	that.info = info;
	info = {};
}
Shader& Shader::operator=(Shader&& that) noexcept {
	if (&that == this) return *this;

	this->~Shader();
	that.info = info;
	info = {};
	return *this;
}

bool Shader::load_vertex(std::filesystem::path path) noexcept {
	auto expected = file::read_whole_text(path);
	if (!expected) return false;

	info.vertexSource = *expected;
	return true;
}
bool Shader::load_fragment(std::filesystem::path path) noexcept {
	auto expected = file::read_whole_text(path);
	if (!expected) return false;

	info.fragmentSource = *expected;
	return true;
}

bool Shader::build_shaders() noexcept {
	auto vertexSource = info.vertexSource.data();
	auto fragmentSource = info.fragmentSource.data();

	glShaderSource(info.vertexId, 1, &vertexSource, nullptr);
	glCompileShader(info.vertexId);
	defer{ glDeleteShader(info.vertexId); info.vertexId = 0; info.vertexSource.clear(); };

	if (auto err = check_shader_error(info.vertexId)) {
		printf("Vertex compilation error: %s\n", err->c_str());
		return false;
	}

	glShaderSource(info.fragmentId, 1, &fragmentSource, nullptr);
	glCompileShader(info.fragmentId);
	defer{ glDeleteShader(info.fragmentId); info.fragmentId = 0; info.fragmentSource.clear(); };

	if (auto err = check_shader_error(info.fragmentId)) {
		printf("Fragment compilation error: %s\n", err->c_str());
		return false;
	}

	info.programId = glCreateProgram();
	glAttachShader(info.programId, info.vertexId);
	glAttachShader(info.programId, info.fragmentId);
	glLinkProgram(info.programId);

	if (auto err = check_program_error(info.programId)) {
		printf("Shader linking error: %s\n", err->c_str());
		return false;
	}

	return true;
}

std::optional<std::string> Shader::check_shader_error(size_t shader) noexcept {
	auto success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::string log(512, 0);
		glGetShaderInfoLog(shader, 512, nullptr, log.data());
		return log;
	}
	return std::nullopt;
}

std::optional<std::string> Shader::check_program_error(size_t program) noexcept {
	auto success = 0;
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::string log(512, 0);
		glGetProgramInfoLog(program, 512, nullptr, log.data());
		return log;
	}
	return std::nullopt;
}

void Shader::use() const noexcept {
	glUseProgram(info.programId);
}

#define loc(x) glGetUniformLocation(info.programId, x)
void Shader::set_primary_color(Vector4d color) noexcept {
	glUniform4dv(loc("color"), 4, &color.x);
}
void Shader::set_rotation(float rotation) noexcept {
	glUniform1f(loc("size"), rotation);
}
void Shader::set_origin(Vector2f origin) noexcept {
	glUniform2fv(loc("origin"), 2, &origin.x);
}
void Shader::set_position(Vector2f pos) noexcept {
	glUniform2fv(loc("pos"), 2, &pos.x);
}
void Shader::set_size(Vector2f size) noexcept {
	glUniform2fv(loc("size"), 2, &size.x);
}
void Shader::set_texture(size_t id) noexcept {
	glUniform1i(loc("color_texture"), (int)id);
}

#undef loc
