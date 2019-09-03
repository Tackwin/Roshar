#include <iostream>
#include <sstream>
#include <fstream>

#include <GL/glew.h>
#include <GL/wglew.h>

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

	info = that.info;
	that.info = {};
}
Shader& Shader::operator=(Shader&& that) noexcept {
	if (&that == this) return *this;

	this->~Shader();
	info = that.info;
	that.info = {};
	return *this;
}

bool Shader::load_vertex(std::filesystem::path path) noexcept {
	auto expected = file::read_whole_text(path);
	if (!expected) return false;

	char* source = (*expected).data();

	if (info.vertex_linked) {
		glDetachShader(info.programId, info.vertexId);
		info.vertex_linked = false;
	}
	if (info.vertex_compiled) {
		glDeleteShader(info.vertexId);
		info.vertexId = glCreateShader(GL_VERTEX_SHADER);
		info.vertex_compiled = false;
	}

	glShaderSource(info.vertexId, 1, &source, nullptr);
	auto x = glGetError();
	if (x != GL_NO_ERROR) {
		printf("%d\n", x);
	}
	glCompileShader(info.vertexId);
	x = glGetError();
	if (x != GL_NO_ERROR) {
		printf("%d\n", x);
	}
	if (auto err = check_shader_error(info.vertexId)) {
		printf("Vertex compilation error: %s\n", err->c_str());
		return false;
	}

	info.vertex_compiled = true;
	return true;
}
bool Shader::load_fragment(std::filesystem::path path) noexcept {
	auto expected = file::read_whole_text(path);
	if (!expected) return false;

	char* source = (*expected).data();

	if (info.fragment_linked) {
		glDetachShader(info.programId, info.fragmentId);
		info.fragment_linked = false;
	}
	if (info.fragment_compiled) {
		glDeleteShader(info.fragmentId);
		info.fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
		info.fragment_compiled = false;
	}


	glShaderSource(info.fragmentId, 1, &source, nullptr);
	glCompileShader(info.fragmentId);

	if (auto err = check_shader_error(info.fragmentId)) {
		printf("Fragment compilation error: %s\n", err->c_str());
		return false;
	}

	info.fragment_compiled = true;
	return true;
}

bool Shader::build_shaders() noexcept {
	info.programId = glCreateProgram();
	glAttachShader(info.programId, info.vertexId);
	glAttachShader(info.programId, info.fragmentId);
	glLinkProgram(info.programId);

	if (auto err = check_program_error(info.programId)) {
		printf("Shader linking error: %s\n", err->c_str());
		return false;
	}

	info.fragment_linked = true;
	info.vertex_linked = true;

	cache_loc.clear();

	return true;
}

std::optional<std::string> Shader::check_shader_error(size_t shader) noexcept {
	auto success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	std::string log(512, 0);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, log.data());
		return log;
	}
	return std::nullopt;
}

std::optional<std::string> Shader::check_program_error(size_t program) noexcept {
	auto success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
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
	Vector4f c = (Vector4f)color;
	glUniform4fv(loc("color"), 1, &c.x);
}
void Shader::set_rotation(float rotation) noexcept {
	glUniform1f(loc("rotation"), rotation);
}
void Shader::set_origin(Vector2f origin) noexcept {
	glUniform2fv(loc("origin"), 1, &origin.x);
}
void Shader::set_position(Vector2f pos) noexcept {
	glUniform2fv(loc("object_pos"), 1, &pos.x);
}
void Shader::set_size(Vector2f size) noexcept {
	glUniform2fv(loc("size"), 1, &size.x);
}
void Shader::set_texture(size_t id) noexcept {
	glUniform1i(loc("color_texture"), (int)id);
}
void Shader::set_view(Rectanglef view) noexcept {
	glUniform4fv(loc("world_bounds"), 1, &view.x);
}
void Shader::set_window_size(Vector2u size) noexcept {
	Vector2f s = (Vector2f)size;
	glUniform2fv(loc("screen_bounds"), 1, &s.x);
}

void Shader::set_use_texture(bool x) noexcept {
	glUniform1i(loc("use_color_texture"), x);
}

void Shader::set_uniform(const std::string& name, Vector2f x) noexcept {
	if (!cache_loc.contains(name)) {
		cache_loc[name] = glGetUniformLocation(info.programId, name.c_str());
	}

	glUniform2fv(cache_loc[name], 1, &x.x);
}
void Shader::set_uniform(const std::string& name, Vector4d x) noexcept {
	if (!cache_loc.contains(name)) {
		cache_loc[name] = glGetUniformLocation(info.programId, name.c_str());
	}

	Vector4f y = (Vector4f)x;
	glUniform4fv(cache_loc[name], 1, &y.x);
}
void Shader::set_uniform(const std::string& name, float x) noexcept {
	if (!cache_loc.contains(name)) {
		cache_loc[name] = glGetUniformLocation(info.programId, name.c_str());
	}

	glUniform1f(cache_loc[name], x);
}
void Shader::set_uniform(const std::string& name, int x) noexcept {
	if (!cache_loc.contains(name)) {
		cache_loc[name] = glGetUniformLocation(info.programId, name.c_str());
	}

	glUniform1i(cache_loc[name], x);
}
void Shader::set_uniform(const std::string& name, Vector4u x) noexcept {
	if (!cache_loc.contains(name)) {
		cache_loc[name] = glGetUniformLocation(info.programId, name.c_str());
	}

	Vector4i y = (Vector4i)x;

	glUniform4iv(cache_loc[name], 4, &y.x);
}
void Shader::set_uniform(const std::string& name, Rectanglef x) noexcept {
	if (!cache_loc.contains(name)) {
		cache_loc[name] = glGetUniformLocation(info.programId, name.c_str());
	}

	glUniform4fv(cache_loc[name], 4, &x.x);
}

#undef loc
