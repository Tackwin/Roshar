#include "Graphics.hpp"

#include <GL/glew.h>

render::View_Info render::current_view;

render::Order render::push_view(Rectanglef bounds) noexcept {
	View_Info info;

	info.world_bounds = bounds;

	Order order;
	order.kind = render::Order::Kind::View_Push;
	order.view = info;

	return order;
}
render::Order render::push_view(Vector2f pos, Vector2f size) noexcept {
	return push_view({ pos, size });
}

render::Order render::pop_view() noexcept {
	Order order;
	order.kind = Order::Kind::View_Pop;
	return order;
}

render::Order render::circle(
	float r,
	Vector2f pos,
	Vector4d color,
	float outline,
	Vector4d outline_color
) noexcept {
	Circle_Info info;
	info.r = r;
	info.pos = pos;
	info.color = color;
	info.outline = outline;
	info.outline_color = outline_color;

	Order order;
	order.circle = info;
	order.kind = Order::Kind::Circle;

	return order;
}

render::Order render::rectangle(
	Vector2f pos,
	Vector2f size,
	Vector4d color,
	float outline,
	Vector4d outline_color
) noexcept {
	Rectangle_Info info;
	info.pos = pos;
	info.size = size;
	info.color = color;
	info.outline = outline;
	info.outline_color = outline_color;

	Order order;
	order.rectangle = info;
	order.kind = Order::Kind::Rectangle;

	return order;
}

render::Order render::sprite(
	Vector2f pos,
	Vector2f size,
	asset::Key texture,
	Vector2f origin,
	float rotation,
	Vector4d color,
	asset::Key shader
) noexcept {
	Sprite_Info info;
	info.pos = pos;
	info.size = size;
	info.origin = origin;
	info.rotation = rotation;
	info.color = color;
	info.texture = texture;
	info.shader = shader ? 0 : asset::Known_Shaders::Default;

	Order order;
	order.sprite = info;
	order.kind = Order::Kind::Sprite;

	return order;
}


void render::immediate(Sprite_Info info) noexcept {
	static GLuint quad_vao{ 0 };
	static GLuint quad_vbo{ 0 };

	if (!quad_vao) {
		static float quad_vertices[] = {
			// positions    
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f
		};

		// setup plane VAO
		glGenVertexArrays(1, &quad_vao);
		glBindVertexArray(quad_vao);
		glGenBuffers(1, &quad_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	glActiveTexture(GL_TEXTURE0);
	if (info.texture)
		glBindTexture(GL_TEXTURE_2D, asset::Store.get_my_texture(info.texture).get_texture_id());

	auto& shader = asset::Store.get_shader(info.shader);
	shader.use();
	shader.set_window_size({ Environment.window_width, Environment.window_height });
	shader.set_view(current_view.world_bounds);
	shader.set_origin(info.origin);
	shader.set_position(info.pos);
	shader.set_primary_color(info.color);
	shader.set_rotation(info.rotation);
	shader.set_size(info.size);
	shader.set_texture(0);
	shader.set_use_texture(true);

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void render::immediate(Circle_Info info) noexcept {
	static GLuint vao{ 0 };
	static GLuint vbo{ 0 };
	constexpr size_t Half_Prec = 10;

	if (!vao) {
		const float ang_inc = 3.1415926f / Half_Prec;
		const float cos_inc = cos(ang_inc);
		const float sin_inc = sin(ang_inc);

		GLfloat vertices[3 * Half_Prec * 2];
		unsigned coord_idx = 0;

		vertices[coord_idx++] = 1.0f;
		vertices[coord_idx++] = 0.0f;
		vertices[coord_idx++] = 1.0f;

		float xc = 1.0f;
		float yc = 0.0f;
		for (unsigned i = 1; i < Half_Prec; ++i) {
			float xc_new = cos_inc * xc - sin_inc * yc;
			yc = sin_inc * xc + cos_inc * yc;
			xc = xc_new;

			vertices[coord_idx++] = xc;
			vertices[coord_idx++] = yc;
			vertices[coord_idx++] = 1;

			vertices[coord_idx++] = xc;
			vertices[coord_idx++] = -yc;
			vertices[coord_idx++] = 1;
		}

		vertices[coord_idx++] = -1.0f;
		vertices[coord_idx++] = 0.0f;
		vertices[coord_idx++] = 1.0f;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(
			GL_ARRAY_BUFFER, 2 * Half_Prec * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW
		);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	auto& shader = asset::Store.get_shader(asset::Known_Shaders::Default);
	shader.use();
	shader.set_window_size({ Environment.window_width, Environment.window_height });
	shader.set_view(current_view.world_bounds);
	shader.set_origin({ 0, 0 });
	shader.set_position(info.pos);
	shader.set_primary_color(info.color);
	shader.set_rotation(0);
	shader.set_size({ info.r, info.r });
	shader.set_texture(0);
	shader.set_use_texture(false);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * Half_Prec);
}

void render::immediate(Rectangle_Info info) noexcept {
	static GLuint quad_vao{ 0 };
	static GLuint quad_vbo{ 0 };

	if (!quad_vao) {
		static float quad_vertices[] = {
			// positions    
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f
		};

		// setup plane VAO
		glGenVertexArrays(1, &quad_vao);
		glBindVertexArray(quad_vao);
		glGenBuffers(1, &quad_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	glActiveTexture(GL_TEXTURE0);

	auto& shader = asset::Store.get_shader(asset::Known_Shaders::Default);
	shader.use();
	shader.set_window_size({ Environment.window_width, Environment.window_height });
	shader.set_view(current_view.world_bounds);
	shader.set_origin({ 0, 0 });
	shader.set_position(info.pos);
	shader.set_primary_color(info.color);
	shader.set_rotation(0);
	shader.set_size(info.size);
	shader.set_use_texture(false);
	shader.set_texture(0);

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
