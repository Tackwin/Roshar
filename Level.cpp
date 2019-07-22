#include "Level.hpp"

#include <imgui.h>
#include <chrono>
#include <thread>

#include "Math/Circle.hpp"
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
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		float size_up = 0.04f;

		shape.setSize(size + V2F(size_up));
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(0.01f);
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, alpha * alpha });
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
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);


		float size_up = 0.04f;

		shape.setSize(size + V2F(size_up));
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(.01f);
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, alpha * alpha });
		shape.setPosition(pos - V2F(size_up / 2));
		target.draw(shape);
	}
}

void Next_Zone::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setFillColor(Vector4f{ 0.1f, 0.1f, 0.1f, 1.f });
	shape.setOutlineColor(Vector4f{ 1.f, 1.f, 1.f, 1.f });
	shape.setOutlineThickness(0.01f);
	shape.setPosition(pos);
	target.draw(shape);

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		float size_up = 0.04f;

		shape.setSize(size + V2F(size_up));
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(.01f);
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, alpha * alpha });
		shape.setPosition(pos - V2F(size_up / 2));
		target.draw(shape);
	}
}

void Dry_Zone::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(rec.size);
	shape.setPosition(rec.pos);
	shape.setFillColor(Vector4d{ 0.5, 0.0, 0.0, 0.5 });
	target.draw(shape);

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		float size_up = 0.04f;

		shape.setSize(rec.size + V2F(size_up));
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(.01f);
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, alpha * alpha });
		shape.setPosition(rec.pos - V2F(size_up / 2));
		target.draw(shape);
	}
}

void Rock::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape shape(r);
	shape.setOrigin(r, r);
	shape.setPosition(pos);
	shape.setFillColor(Vector4d{ 0.5, 0.6, 0.7, 1.0 });
	target.draw(shape);

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		shape.setRadius(r + 0.04f);
		shape.setOrigin(shape.getRadius(), shape.getRadius());
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, alpha * alpha });
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

	ImGui::Begin("Dispenser");
	ImGui::PushID(this);
	ImGui::Text("%f", timer);
	ImGui::PopID();
	ImGui::End();

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
	for (auto& x : rocks) x.render(target);
	for (auto& x : dry_zones) x.render(target);
	for (auto& x : kill_zones) x.render(target);
	for (auto& x : next_zones) x.render(target);
	for (auto& x : dispensers) x->render(target);
	for (auto& x : projectiles) x.render(target);
	for (auto& x : prest_sources) x.render(target);

	for (auto& x : markers) {
		sf::CircleShape shape(0.05f);
		shape.setPosition(x);
		shape.setFillColor(sf::Color::Red);
		target.draw(shape);
	}

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
		Vector2f::renderLine(
			target,
			target.mapPixelToCoords(*start_drag),
			IM::getMousePosInView(camera),
			{ 0, 1, 0, 1 }
		);
	}

	static bool flag = false;
	for (const auto& x : debug_vectors) {
		Vector2f::renderArrow(target, x.a, x.a + x.b, { 1, 1, 0, 1 }, { 1, 1, 0, 1 });
	}
	if (flag) {
		using namespace std::chrono_literals;
	}
	flag = !debug_vectors.empty();


	auto view = target.getView();
	defer{ target.setView(view); };
	target.setView(target.getDefaultView());
	sf::RectangleShape shape(
		{ (float)Environment.window_width, (float)Environment.window_height }
	);
	shape.setPosition(0, 0);
	if (!in_editor && camera_fade_out_timer > 0) {
		auto alpha = 1 - (camera_fade_out_timer / Camera_Fade_Time);

		shape.setFillColor(Vector4d{ 0, 0, 0, alpha });
		target.draw(shape);
	}
	if (!in_editor && camera_fade_out_timer <= 0 && camera_fade_in_timer > 0) {
		auto alpha = camera_fade_in_timer / Camera_Fade_Time;
		auto gray = 1 - alpha;

		shape.setFillColor(Vector4d{ gray, gray, gray, alpha });
		target.draw(shape);
	}
}

Level::Level() noexcept {
	camera.setSize({ 10, -(IM::getWindowSize().y / (float)IM::getWindowSize().x) });
}

bool Level::test_input(float) noexcept {
	if (IM::isWindowFocused()) {
		if (IM::isKeyJustPressed(sf::Keyboard::Quote)) {
			retry();
			return true;
		}

		if (IM::isKeyPressed(sf::Keyboard::Q)) {
			player.flat_velocities.push_back({ -5, 0 });
		}
		if (IM::isKeyPressed(sf::Keyboard::D)) {
			player.flat_velocities.push_back({ +5, 0 });
		}
		if (IM::isKeyJustPressed(sf::Keyboard::Space) && player.floored) {
			player.velocity += Vector2f{ 0, +7.5 };
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
		if (IM::isKeyJustPressed(sf::Keyboard::Return)) {
			markers.push_back(player.pos);

			if (IM::isKeyPressed(sf::Keyboard::LShift)) markers.clear();
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
				discrete_angle = angle_step * std::round(discrete_angle / angle_step);

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
	return false;
}

void Level::update(float dt) noexcept {
	debug_vectors.clear();
	update_camera(dt);

	if (camera_fade_out_timer > 0) {
		camera_fade_out_timer -= dt;
		if (camera_fade_out_timer <= 0.f) {
			retry();
			camera_fade_in_timer = Camera_Fade_Time;
			return;
		}
	}
	if (camera_fade_out_timer <= 0.f && camera_fade_in_timer > 0) {
		camera_fade_in_timer -= dt;
	}
	
	if (input_active_timer > 0) {
		input_active_timer -= dt;
		if (input_active_timer > 0.f) return;
	}

	
	auto previous_player = player;

	if (test_input(dt) || in_editor) return;

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
	player.velocity *= std::powf(Environment.drag * (player.floored ? 0.1f : 1.f), dt);

	player.forces = {};

	test_collisions(dt, previous_player);
}

void Level::test_collisions(float dt, Player previous_player) noexcept {
	const auto dead_velocity_2 = Environment.dead_velocity * Environment.dead_velocity;
	auto test_any = [](const auto& cont, const auto& to_test) {
		return std::any_of(BEG_END(cont), [&](auto x) {return test(x, to_test); });
	};
	auto test_any_p = [&p = player, test_any](const auto& cont) { return test_any(cont, p); };

	Vector2f flat_velocities = { 0, 0 };
	for (const auto& x : player.flat_velocities) flat_velocities += x;
	player.flat_velocities.clear();

	const auto velocities = flat_velocities + player.velocity;
	float impact = 0;

	player.pos.x += velocities.x * dt;
	if (test_any_p(blocks)) {
		impact += std::abs(velocities.x);
		player.pos.x = previous_player.pos.x;
		player.velocity.x = 0;
		player.forces.x = 0;
	}
	player.floored = false;
	player.pos.y += velocities.y * dt;
	if (test_any_p(blocks)) {
		impact += std::abs(velocities.y);
		player.pos.y = previous_player.pos.y;
		player.floored = velocities.y < 0;
		player.velocity.y = 0;
		player.forces.y = 0;
	}
	
	for (auto& rock : rocks) {
		for (const auto& x : rock.bindings) rock.velocity += x * dt / rock.mass;
		rock.velocity.y -= Environment.gravity * dt;

		Circlef circle;

		circle.c = rock.pos;
		circle.r = rock.r;

		circle.c += rock.velocity * dt;

		Vector2f debug_1;
		Vector2f debug_2;

		size_t max_loop = 10;
		for (const auto& block : blocks) {
			Rectanglef rec;

			rec.pos = block.pos;
			rec.size = block.size;

			if (auto opt = get_next_velocity(circle, rock.velocity, rec, 0.f); opt) {
				debug_1 = rock.velocity;
				debug_2 = *opt;
				rock.velocity = *opt;
				circle.c = rock.pos;
				//circle.c = rock.pos + rock.velocity * dt;
			}
		}

		debug_vectors.push_back({ rock.pos, debug_1 });
		debug_vectors.push_back({ rock.pos, debug_2 });

		rock.pos = circle.c;
		continue;
		circle.c.x += rock.velocity.x * dt;

		for (auto& block : blocks) {
			Rectanglef rec;

			rec.pos = block.pos;
			rec.size = block.size;

			if (is_in(rec, circle)) {
				circle.c.x = rock.pos.x;
				rock.velocity.x = 0;
				break;
			}
		}

		circle.c.y += rock.velocity.y * dt;

		for (auto& block : blocks) {
			Rectanglef rec;

			rec.pos = block.pos;
			rec.size = block.size;

			if (is_in(rec, circle)) {
				circle.c.y = rock.pos.y;
				rock.velocity.y = 0;
				break;
			}
		}

		rock.pos = circle.c;
	}

	if (impact > Environment.dead_velocity || test_any_p(kill_zones) || test_any_p(projectiles)) {
		return die();
	}

	for (const auto& d : dry_zones) {
		if (test(d, player)) basic_bindings.clear();
		for (auto& r : rocks) {
			if (test(d, r)) r.bindings.clear();
		}
	}

	for (auto& x : next_zones) {
		if (!test(x, player)) continue;

		auto opt_dyn = load_from_json_file(Exe_Path / LEVEL_PATH / x.next_level);
		if (!opt_dyn) {
			printf("Couldn't load file: %s%s\n", LEVEL_PATH, x.next_level.c_str());
			continue;
		}

		*this = (Level)*opt_dyn;
		return;
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
	for (auto& x : dispensers) {
		x->timer = 1.f / x->hz - x->offset_timer;
	}
}

void Level::die() noexcept {
	input_active_timer = Input_Active_Time;
	camera_fade_out_timer = Camera_Fade_Time;
	camera_fade_in_timer = Camera_Fade_Time;
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
void from_dyn_struct(const dyn_struct& str, Dry_Zone& x) noexcept {
	x.rec = (Rectanglef)str["rec"];
}
void to_dyn_struct(dyn_struct& str, const Dry_Zone& x) noexcept {
	str = dyn_struct::structure_t{};
	str["rec"] = x.rec;
}
void from_dyn_struct(const dyn_struct& str, Dispenser& dispenser) noexcept {
	if (has(str, "offset_timer")) dispenser.offset_timer = (float)str["offset_timer"];
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
	str["offset_timer"] = dispenser.offset_timer;
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
	for (const auto& x : iterate_array(str["next_zones"]))
		level.next_zones.push_back((Next_Zone)x);
	if (has(str, "dry_zones")) for (const auto& x : iterate_array(str["dry_zones"]))
		level.dry_zones.push_back((Dry_Zone)x);
	if (has(str, "rocks")) for (const auto& x : iterate_array(str["rocks"]))
		level.rocks.push_back((Rock)x);
	for (const auto& x : iterate_array(str["prest_sources"]))
		level.prest_sources.push_back((Prest_Source)x);
	for (const auto& x : iterate_array(str["dispensers"]))
		level.dispensers.push_back(std::make_shared<Dispenser>(x));
	if (has(str, "markers")) {
		for (const auto& x : iterate_array(str["markers"]))
			level.markers.push_back((Vector2f)x);
	}


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
	str["next_zones"] = dyn_struct_array();
	for (const auto& x : level.next_zones) str["next_zones"].push_back(x);
	str["dry_zones"] = dyn_struct_array();
	for (const auto& x : level.dry_zones) str["dry_zones"].push_back(x);
	str["prest_sources"] = dyn_struct_array();
	for (const auto& x : level.prest_sources) str["prest_sources"].push_back(x);
	str["dispensers"] = dyn_struct_array();
	for (const auto& x : level.dispensers) str["dispensers"].push_back(*x);
	str["markers"] = dyn_struct_array();
	for (const auto& x : level.markers) str["markers"].push_back(x);
	str["rocks"] = dyn_struct_array();
	for (const auto& x : level.rocks) str["rocks"].push_back(x);

	auto& player = str["player"] = dyn_struct::structure_t{};

	player["prest"] = level.player.prest;
	player["pos"] = level.player.pos;
	player["size"] = level.player.size;

	auto& camera = str["camera"] = dyn_struct::structure_t{};

	camera["pos"] = (Vector2f)level.camera.getCenter();
	camera["size"] = (Vector2f)level.camera.getSize();
}
void from_dyn_struct(const dyn_struct& str, Next_Zone& x) noexcept {
	x.pos = (Vector2f)str["pos"];
	x.size = (Vector2f)str["size"];
	x.next_level = (std::string)str["next_level"];
}
void to_dyn_struct(dyn_struct& str, const Next_Zone& x) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = x.pos;
	str["size"] = x.size;
	str["next_level"] = x.next_level;
}
void from_dyn_struct(const dyn_struct& str, Rock& x) noexcept {
	x.pos = (Vector2f)str["pos"];
	x.velocity = (Vector2f)str["velocity"];
	x.r = (float)str["r"];
	x.mass = (float)str["mass"];
}
void to_dyn_struct(dyn_struct& str, const Rock& x) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = x.pos;
	str["velocity"] = x.velocity;
	str["mass"] = x.mass;
	str["r"] = x.r;
}


