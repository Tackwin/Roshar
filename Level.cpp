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

		float size_up = 0.04f;

		shape.setSize(size + V2F(size_up));
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(0.01f);
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, sin_time });
		shape.setPosition(pos - V2F(size_up / 2));
		target.draw(shape);
	}
}

void Kill_Zone::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setFillColor(Vector4f{ 0.1f, 1.f, 0.5f, 1.f });
	shape.setOutlineColor(Vector4f{ 1.f, 1.f, 1.f, 1.f });
	shape.setOutlineThickness(0.01f);
	shape.setPosition(pos);
	target.draw(shape);

	if (editor_selected) {
		thread_local float sin_time = 0;
		sin_time += std::cosf(sin_time) * 0.01f;

		float size_up = 0.04f;

		shape.setSize(size + V2F(size_up));
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(.01f);
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

void Dispenser::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape shape(proj_r);
	shape.setFillColor(Vector4f{ 0.4f, 0.3f, 0.2f, 1.f });
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setPosition(start_pos);
	target.draw(shape);

	if (editor_selected) {
		shape.setOutlineThickness(0.1f);
		target.draw(shape);

		shape.setFillColor(sf::Color::Transparent);
		shape.setPosition(end_pos);
		target.draw(shape);

		std::size_t i = 0;
		auto proj_timer = offset_timer + ++i / hz - timer;

		while ((end_pos - start_pos).length() > proj_speed * proj_timer && proj_timer > 0) {
			Projectile p;
			p.pos = start_pos + (end_pos - start_pos).normalize() * proj_timer * proj_speed;
			p.r = proj_r;
			p.render(target);
			proj_timer = offset_timer + ++i / hz - timer;
		}
	}
}

void Projectile::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape shape(r);
	shape.setFillColor(Vector4f{ 0.2f, 0.3f, 0.4f, 1.f });
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setPosition(pos);
	target.draw(shape);
}

void Level::render(sf::RenderTarget& target) noexcept {
	target.setView(camera);
	for (auto& x : blocks) x.render(target);
	for (auto& x : kill_zones) x.render(target);
	for (auto& x : dispensers) x->render(target);
	for (auto& x : projectiles) x.render(target);
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


bool blockPlayer(std::vector<Block>& blocks, const Player& player) noexcept {
	for (const auto& b : blocks) if (test(b, player)) return true;
	return false;
}

bool killZonesPlayer(std::vector<Kill_Zone>& kill_zones, const Player& player) noexcept {
	for (const auto& b : kill_zones) if (test(b, player)) return true;
	return false;
}

bool projectilsPlayer(std::vector<Projectile>& projectiles, const Player& player) noexcept {
	return std::any_of(BEG_END(projectiles), [&](auto x) {return test(x, player); });
}

Level::Level() noexcept {
	camera.setSize({ 10, -(IM::getWindowSize().y / (float)IM::getWindowSize().x) });
}

void Level::update(float dt) noexcept {
	update_camera(dt);
	started |= IM::isKeyJustPressed();
	if (!started) return;
	
	auto prev_player_pos = player.pos;

	auto prevPlayer = player;

	if (IM::isWindowFocused()) {
		if (IM::isKeyJustPressed(sf::Keyboard::Quote)) {
			return retry();
		}

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
				discrete_angle = angle_step * std::roundf(discrete_angle / angle_step);

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

	for (auto& x : dispensers) {
		x->timer -= dt;
		if (x->timer <= 0) {
			x->timer = 1 / x->hz;

			Projectile p;
			p.pos = x->start_pos;
			p.r = x->proj_r;
			p.speed = (x->end_pos - x->start_pos).normalize() * x->proj_speed;
			p.origin = x;

			projectiles.push_back(p);
		}
	}
	for (size_t i = projectiles.size() - 1; i + 1 > 0; --i) {
		auto& x = projectiles[i];

		x.pos += x.speed * dt;

		if (x.origin.expired()) continue;

		if (test(*x.origin.lock(), x)) {
			projectiles.erase(BEG(projectiles) + i);
		}
	}

	player.forces.y -= Environment.gravity;
	for (const auto& x : basic_bindings) player.forces += x * Environment.gravity;

	player.velocity += player.forces * dt;
	player.velocity *= std::powf(Environment.drag, dt);
	player.forces = {};

	const auto dead_velocity_2 = Environment.dead_velocity * Environment.dead_velocity;

	player.pos.x += player.velocity.x * dt;
	if (blockPlayer(blocks, player)) {
		if (player.velocity.length2() > dead_velocity_2) return retry();
		player.pos.x = prevPlayer.pos.x;
		player.velocity.x = 0;
		player.forces.x = 0;
	}
	if (killZonesPlayer(kill_zones, player) || projectilsPlayer(projectiles, player)) {
		return retry();
	}
	player.floored = false;
	player.pos.y += player.velocity.y * dt;
	if (blockPlayer(blocks, player)) {
		if (player.velocity.length2() > dead_velocity_2) return retry();
		player.pos.y = prevPlayer.pos.y;
		player.floored = player.velocity.y < 0;
		player.velocity.y = 0;
		player.forces.y = 0;
	}
	if (killZonesPlayer(kill_zones, player) || projectilsPlayer(projectiles, player)) {
		return retry();
	}

	for (size_t i = prest_sources.size() - 1; i + 1 > 0; --i) {
		if (test(prest_sources[i], player)) {
			player.prest += prest_sources[i].prest;
			prest_sources.erase(BEG(prest_sources) + i);
			break;
		}
	}
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


void Level::pause() noexcept {}
void Level::resume() noexcept {
	auto iter = [](auto& x) noexcept { for (auto& y : x) y.editor_selected = false; };
	auto iter_ptr = [](auto& x) noexcept { for (auto& y : x) y->editor_selected = false; };

	iter(blocks);
	iter(kill_zones);
	iter(prest_sources);
	iter_ptr(dispensers);
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
void from_dyn_struct(const dyn_struct& str, Dispenser& dispenser) noexcept {
	dispenser.start_pos = (Vector2f)str["start_pos"];
	dispenser.end_pos = (Vector2f)str["end_pos"];
	dispenser.proj_r = (float)str["proj_r"];
	dispenser.hz = (float)str["hz"];
	dispenser.proj_speed = (float)str["proj_speed"];
}
void to_dyn_struct(dyn_struct& str, const Dispenser& dispenser) noexcept {
	str = dyn_struct::structure_t{};
	str["start_pos"] = dispenser.start_pos;
	str["end_pos"] = dispenser.end_pos;
	str["proj_r"] = dispenser.proj_r;
	str["proj_speed"] = dispenser.proj_speed;
	str["hz"] = dispenser.hz;
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
	for (const auto& x : iterate_array(str["dispensers"]))
		level.dispensers.push_back(std::make_shared<Dispenser>(x));

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
	str["dispensers"] = dyn_struct_array();
	for (const auto& x : level.dispensers) str["dispensers"].push_back(*x);

	auto& player = str["player"] = dyn_struct::structure_t{};

	player["prest"] = level.player.prest;
	player["pos"] = level.player.pos;
	player["size"] = level.player.size;

	auto& camera = str["camera"] = dyn_struct::structure_t{};

	camera["pos"] = (Vector2f)level.camera.getCenter();
	camera["size"] = (Vector2f)level.camera.getSize();
}
