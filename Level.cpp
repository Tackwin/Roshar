#include "Level.hpp"

#include "Managers/InputsManager.hpp"
#include "Collision.hpp"
#include "Time.hpp"

void Block::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
	shape.setOutlineThickness(1);
	shape.setPosition(pos);
	target.draw(shape);

	if (editor_selected) {
		thread_local float sin_time = 0;
		sin_time += std::cosf(sin_time) * 0.01f;

		float size_up = 4;

		shape.setSize(size + V2F(size_up));
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(1);
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, sin_time });
		shape.setPosition(pos - V2F(size_up / 2));
		target.draw(shape);
	}
}

void Player::update(float dt) noexcept {
}

void Player::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.0, 0.0, 0.0, 1.0 });
	shape.setOutlineThickness(1);
	shape.setPosition(pos);
	shape.setFillColor(sf::Color::Cyan);
	target.draw(shape);
}

void Level::render(sf::RenderTarget& target) noexcept {
	target.setView(camera);
	for (auto& x : blocks) x.render(target);
	player.render(target);

	if (start_drag) {
		auto time_dt = (int)(std::ceil(4.f * (seconds() - drag_time))) * 10;
		sf::CircleShape shape;
		shape.setRadius(time_dt);
		shape.setOrigin(time_dt, time_dt);
		shape.setPosition(IM::getMousePosInView(camera));
		shape.setFillColor(sf::Color::Green);
		shape.setOutlineThickness(1);
		shape.setOutlineColor(sf::Color::White);
		target.draw(shape);
	}

	for (auto& binding : basic_bindings) {
		Vector2f::renderArrow(
			target,
			player.pos + player.size / 2,
			player.pos + player.size / 2 + binding * 100,
			{ 1, 0, 1, 1 },
			{ 1, 0, 1, 1 }
		);
	}

	if (start_drag) {
		Vector2f::renderLine(target, target.mapPixelToCoords(*start_drag), IM::getMousePosInView(camera), { 0, 1, 0, 1 });
	}
}


bool blockPlayer(std::vector<Block>& blocks, Player player) noexcept {
	for (const auto& b : blocks) if (test(b, player)) return true;
	return false;
}

Level::Level() noexcept {
	camera.setSize({ 1280, -720 });
}

void Level::update(float dt) noexcept {
	auto prev_player_pos = player.pos;

	auto prevPlayer = player;
	if (IM::isKeyPressed(sf::Keyboard::Q)) {
		player.pos.x -= dt * 500;
	}
	if (IM::isKeyPressed(sf::Keyboard::D)) {
		player.pos.x += dt * 500;
	}
	if (IM::isKeyJustPressed(sf::Keyboard::Space)) {
		player.velocity.y += 750;
	}
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		start_drag = IM::getMouseScreenPos();
		drag_time = seconds();
	}
	if (IM::isMouseJustPressed(sf::Mouse::Right)) basic_bindings.clear();
	if (start_drag) {
		if (!IM::isMousePressed(sf::Mouse::Left)) {
			start_drag.reset();
		}
		auto new_pos = IM::getMouseScreenPos();
		auto dt_vec = new_pos - *start_drag;
		dt_vec.y *= -1;
		if (dt_vec.length2() > drag_dead_zone * drag_dead_zone) {
			float discrete_angle = dt_vec.angleX();
			float angle_step = 2 * PI / Environment.drag_angle_step;
			discrete_angle = angle_step * (int)(discrete_angle / angle_step);

			basic_bindings.push_back(
				Vector2f::createUnitVector(discrete_angle) *
				(int)(std::ceil(4 * (seconds() - drag_time))) / 2.f
			);
			start_drag.reset();
		}
	}


	player.forces.y -= Environment.gravity;
	for (const auto& x : basic_bindings) player.forces += x * Environment.gravity;

	player.velocity += player.forces * dt;
	player.velocity *= std::powf(Environment.drag, dt);
	player.forces = {};

	player.pos.x += player.velocity.x * dt;
	if (blockPlayer(blocks, player)) {
		player.pos.x = prevPlayer.pos.x;
		player.velocity.x = 0;
		player.forces.x = 0;
	}
	player.pos.y += player.velocity.y * dt;
	if (blockPlayer(blocks, player)) {
		player.pos.y = prevPlayer.pos.y;
		player.velocity.y = 0;
		player.forces.y = 0;
	}

	update_camera(dt);
}

void Level::update_camera(float dt) noexcept {
	auto camera_idle_radius_2 = camera_idle_radius * camera_idle_radius;
	if (camera_target) {
		Vector2f pos = camera.getCenter();
		if ((pos - *camera_target).length2() < camera_idle_radius_2) return camera_target.reset();

		Vector2f dt_pos = (*camera_target - pos);

		camera.move(std::fminf(dt_pos.length(), camera_speed) * dt_pos.normalize());
	}
	Vector2f pos = camera.getCenter();
	Vector2f target = player.pos;
	if ((target - pos).length2() > camera_idle_radius_2) camera_target = target;
;}
