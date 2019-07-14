#include "Level.hpp"

#include "Managers/InputsManager.hpp"
#include "Collision.hpp"
#include "Time.hpp"

void Block::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
	shape.setOutlineThickness(0.01f);
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

void Kill_Zone::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 0.1f, 1.f, 0.5f, 1.f });
	shape.setOutlineThickness(0.01f);
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

void Player::update(float) noexcept {
}

void Player::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.0, 0.0, 0.0, 1.0 });
	shape.setOutlineThickness(0.01f);
	shape.setPosition(pos);
	shape.setFillColor(sf::Color::Cyan);
	target.draw(shape);
}

void Level::render(sf::RenderTarget& target) noexcept {
	target.setView(camera);
	for (auto& x : blocks) x.render(target);
	for (auto& x : kill_zones) x.render(target);
	for (auto& x : prest_sources) x.render(target);
	player.render(target);

	if (start_drag) {
		auto time_dt =
			std::ceilf((float)(Environment.gather_speed * (seconds() - drag_time))) *
			Environment.gather_step;
		float prest_gathered = std::min(time_dt, player.prest) * 0.1f;
		sf::CircleShape shape;
		shape.setRadius(prest_gathered);
		shape.setOrigin(prest_gathered, prest_gathered);
		shape.setPosition(IM::getMousePosInView(camera));
		shape.setFillColor(sf::Color::Green);
		shape.setOutlineThickness(0.01f);
		shape.setOutlineColor(sf::Color::White);
		target.draw(shape);
	}

	for (auto& binding : basic_bindings) {
		Vector2f::renderArrow(
			target,
			player.pos + player.size / 2,
			player.pos + player.size / 2 + binding,
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

bool killZonesPlayer(std::vector<Kill_Zone>& kill_zones, Player player) noexcept {
	for (const auto& b : kill_zones) if (test(b, player)) return true;
	return false;
}

Level::Level() noexcept {
	camera.setSize({ 10, -(IM::getWindowSize().y / (float)IM::getWindowSize().x) });
}

void Level::update(float dt) noexcept {
	auto prev_player_pos = player.pos;

	auto prevPlayer = player;

	if (IM::isWindowFocused()) {
		if (IM::isKeyPressed(sf::Keyboard::Q)) {
			player.pos.x -= dt * 5;
		}
		if (IM::isKeyPressed(sf::Keyboard::D)) {
			player.pos.x += dt * 5;
		}
		if (IM::isKeyJustPressed(sf::Keyboard::Space) && player.floored) {
			player.velocity.y += 7.5f;
		}
		if (IM::isMouseJustPressed(sf::Mouse::Left)) {
			start_drag = IM::getMouseScreenPos();
			drag_time = seconds();
		}
		if (IM::isMouseJustPressed(sf::Mouse::Right)) basic_bindings.clear();
		if (
			IM::isKeyPressed(sf::Keyboard::LControl) &&
			IM::isKeyJustPressed(sf::Keyboard::Z) &&
			!basic_bindings.empty()
		) {
			basic_bindings.pop_back();
		}

		if (start_drag) {
			if (!IM::isMousePressed(sf::Mouse::Left)) {
				start_drag.reset();
			}
			auto new_pos = IM::getMouseScreenPos();
			auto dt_vec = new_pos - *start_drag;
			dt_vec.y *= -1;
			if (dt_vec.length2() > drag_dead_zone * drag_dead_zone) {
				auto discrete_angle = dt_vec.angleX();
				auto angle_step = 2 * PI / Environment.drag_angle_step;
				discrete_angle = angle_step * (int)(discrete_angle / angle_step);

				auto unit = Vector2f::createUnitVector(discrete_angle);
				auto prest_gathered = 
					(int)(std::ceil(Environment.gather_speed * (seconds() - drag_time))) *
					Environment.gather_step;
				prest_gathered = std::min(prest_gathered, player.prest);
				if (prest_gathered != 0) {
					player.prest -= prest_gathered;
					basic_bindings.push_back(unit * prest_gathered);
				}
				start_drag.reset();
			}
		}
	}


	player.forces.y -= Environment.gravity;
	for (const auto& x : basic_bindings) player.forces += x * Environment.gravity;

	player.velocity += player.forces * dt;
	player.velocity *= std::powf(Environment.drag, dt);
	player.forces = {};

	player.pos.x += player.velocity.x * dt;
	if (killZonesPlayer(kill_zones, player)) {
		return retry();
	}
	if (blockPlayer(blocks, player)) {
		player.pos.x = prevPlayer.pos.x;
		player.velocity.x = 0;
		player.forces.x = 0;
	}
	player.floored = false;
	player.pos.y += player.velocity.y * dt;
	if (killZonesPlayer(kill_zones, player)) {
		return retry();
	}
	if (blockPlayer(blocks, player)) {
		player.pos.y = prevPlayer.pos.y;
		player.floored = player.velocity.y < 0;
		player.velocity.y = 0;
		player.forces.y = 0;
	}

	for (size_t i = prest_sources.size() - 1; i + 1 > 0; --i) {
		if (test(prest_sources[i], player)) {
			player.prest += prest_sources[i].prest;
			prest_sources.erase(BEG(prest_sources) + i);
			break;
		}
	}

	update_camera(dt);
}

void Level::update_camera(float dt) noexcept {
	auto camera_idle_radius_2 = camera_idle_radius * camera_idle_radius;
	if (camera_target) {
		Vector2f pos = camera.getCenter();
		if ((pos - *camera_target).length2() < camera_idle_radius_2) return camera_target.reset();

		Vector2f dt_pos = (*camera_target - pos);

		camera.move(dt * camera_speed * dt_pos.normalize());
	}
	Vector2f pos = camera.getCenter();
	Vector2f target = player.pos;
	if ((target - pos).length2() > camera_idle_radius_2) camera_target = target;
}

void Level::retry() noexcept {
	if (initial_level) {
		initial_level->initial_level = new Level(*initial_level);
		auto new_level = *initial_level;
		*this = new_level;
	}
}

void Prest_Source::render(sf::RenderTarget& target) noexcept {
	auto r = std::sqrt(prest) * Radius_Multiplier;

	sf::CircleShape shape;
	shape.setRadius(r);
	shape.setOrigin(r, r);
	shape.setPosition(pos);
	shape.setFillColor(sf::Color::Cyan);
	target.draw(shape);

	if (!editor_selected) return;

	shape.setRadius(r + 0.04f);
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setPosition(pos);
	shape.setFillColor(Vector4d{ 1.0, 1.0, 1.0, 0.1 });
	target.draw(shape);
}

void from_dyn_struct(const dyn_struct& str, Block& block) noexcept {
	block.pos = (Vector2f)str["pos"];
	block.size = (Vector2f)str["size"];
}
void to_dyn_struct(dyn_struct& str, const Block& block) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = block.pos;
	str["size"] = block.size;
}
void from_dyn_struct(const dyn_struct& str, Kill_Zone& block) noexcept {
	block.pos = (Vector2f)str["pos"];
	block.size = (Vector2f)str["size"];
}
void to_dyn_struct(dyn_struct& str, const Kill_Zone& block) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = block.pos;
	str["size"] = block.size;
}
void from_dyn_struct(const dyn_struct& str, Prest_Source& prest) noexcept {
	prest.pos = (Vector2f)str["pos"];
	prest.prest = (float)str["prest"];
}
void to_dyn_struct(dyn_struct& str, const Prest_Source& prest) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = prest.pos;
	str["prest"] = prest.prest;
}
void from_dyn_struct(const dyn_struct& str, Level& level) noexcept {
	for (const auto& x : iterate_array(str["blocks"])) level.blocks.push_back((Block)x);
	for (const auto& x : iterate_array(str["kill_zones"]))
		level.kill_zones.push_back((Kill_Zone)x);
	for (const auto& x : iterate_array(str["prest_sources"]))
		level.prest_sources.push_back((Prest_Source)x);

	Player player;
	player.forces = {};
	player.velocity = {};
	player.prest = (float)str["player"]["prest"];
	player.pos = (Vector2f)str["player"]["pos"];
	player.size = (Vector2f)str["player"]["size"];
	level.player = player;

	level.camera.setCenter((Vector2f)str["camera"]["pos"]);
	level.camera.setSize((Vector2f)str["camera"]["size"]);

	level.initial_level = new Level(level);
}
void to_dyn_struct(dyn_struct& str, const Level& level) noexcept {
	str = dyn_struct::structure_t{};
	str["blocks"] = dyn_struct_array();
	for (const auto& x : level.blocks) str["blocks"].push_back(x);
	str["kill_zones"] = dyn_struct_array();
	for (const auto& x : level.kill_zones) str["kill_zones"].push_back(x);
	str["prest_sources"] = dyn_struct_array();
	for (const auto& x : level.prest_sources) str["prest_sources"].push_back(x);

	auto& player = str["player"] = dyn_struct::structure_t{};

	player["prest"] = level.player.prest;
	player["pos"] = level.player.pos;
	player["size"] = level.player.size;

	auto& camera = str["camera"] = dyn_struct::structure_t{};

	camera["pos"] = (Vector2f)level.camera.getCenter();
	camera["size"] = (Vector2f)level.camera.getSize();
}
