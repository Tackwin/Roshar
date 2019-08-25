#include "Graphics.hpp"

#include <GL/glew.h>


void render::sprite(Vector2f pos, Vector2f size, asset::Key texture) noexcept {
	Sprite_Info info;
	info.pos = pos;
	info.size = size;
	info.origin = { 0, 0 };
	info.rotation = 0;
	info.color = { 1, 1, 1, 1 };
	info.texture = texture;
	info.shader = asset::Known_Shaders::Default;

	render::sprite(info);
}
void render::sprite(Sprite_Info info) noexcept {
	static size_t quad_vao{ 0 };
	static size_t quad_vbo{ 0 };

	if (!quad_vao) {

		float quad_vertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		// setup plane VAO
		glGenVertexArrays(1, &quad_vao);
		glGenBuffers(1, &quad_vbo);
		glBindVertexArray(quad_vao);
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
