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

render::Order render::sprite(Vector2f pos, Vector2f size, asset::Key texture) noexcept {
	Sprite_Info info;
	info.pos = pos;
	info.size = size;
	info.origin = { 0, 0 };
	info.rotation = 0;
	info.color = { 1, 1, 1, 1 };
	info.texture = texture;
	info.shader = asset::Known_Shaders::Default;

	Order order;
	order.sprite = info;
	order.kind = Order::Kind::Sprite;

	return order;
}

float quad_vertices[] = {
	// positions        // texture Coords
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
};

void render::sprite(Sprite_Info info) noexcept {
	static GLuint quad_vao{ 0 };
	static GLuint quad_vbo{ 0 };
	

	if (!quad_vao) {
		// setup plane VAO
		glGenVertexArrays(1, &quad_vao);
		glBindVertexArray(quad_vao);
		glGenBuffers(1, &quad_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))
		);
	}

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, asset::Store.get_my_texture(info.texture).get_texture_id());

	if (!asset::Store.shaders.contains(info.shader)) return;
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

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
