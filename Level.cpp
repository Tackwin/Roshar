#include "Level.hpp"

#include <imgui.h>
#include <chrono>
#include <thread>

#include "Math/Circle.hpp"
#include "Collision.hpp"
#include "Time.hpp"
#include "OS/file.hpp"
#include "Game.hpp"

void match_and_destroy_keys(Player& p, Door& d) noexcept;

void Block::render(render::Orders& target) const noexcept {
	target.push_rectangle(pos, size, {1, 1, 1, 1});

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sin((sin_time += 1) * 0.001);

		float size_up = 0.04f;

		target.push_rectangle(
			pos - V2F(size_up / 2),
			size + V2F(size_up / 2),
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

void Kill_Zone::render(render::Orders& target) const noexcept {
	target.push_rectangle(pos, size, { 0.1, 1, 0.5, 1 });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);


		float size_up = 0.04f;

		target.push_rectangle(
			pos - V2F(size_up / 2),
			size + V2F(size_up / 2),
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

void Next_Zone::render(render::Orders& target) const noexcept {
	target.push_rectangle(pos, size, { 0.1, 0.1, 0.1, 1 });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		float size_up = 0.04f;

		target.push_rectangle(
			pos - V2F(size_up / 2),
			size + V2F(size_up / 2),
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

void Dry_Zone::render(render::Orders& target) const noexcept {
	target.push_rectangle(rec, { 0.5, 0.0, 0.0, 0.5 });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		float size_up = 0.04f;

		target.push_rectangle(
			rec.pos - V2F(size_up / 2),
			rec.size + V2F(size_up / 2),
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

void Rock::render(render::Orders& target) const noexcept {
	target.push_sprite(pos, { 2 * r, 2 * r }, asset::Known_Textures::Rock, { .5, .5 });

	for (auto& binding : bindings) target.push_arrow(pos, pos + binding, { 1, 0, 1, 1 });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		target.push_circle(
			r,
			pos,
			{ 1., 1., 1., alpha * alpha }
		);
	}
}

void Trigger_Zone::render(render::Orders& target) const noexcept {
	target.push_rectangle(rec, { 1, 0.0, 1, 1 });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		target.push_rectangle(
			rec,
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

void Door::render(render::Orders& target) const noexcept {
	target.push_rectangle(rec, { 0.9, 0.9, 0.9, closed ? 1.0 : 0.1 });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		target.push_rectangle(
			rec,
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

void Friction_Zone::render(render::Orders& target) const noexcept {
	target.push_rectangle(rec, { std::atan(friction) * 0.9, std::atan(friction) * 0.9, 0.1, 1 });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		target.push_rectangle(
			rec,
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

void Prest_Source::render(render::Orders& target) const noexcept {
	auto r = std::sqrt(prest) * Radius_Multiplier;

	if (editor_selected) target.push_circle(r + 0.04f, pos, { 1.0, 1.0, 1.0, 0.1 });
	else                 target.push_circle(r, pos, { 0, 1, 1, 1 });
}

void Auto_Binding_Zone::render(render::Orders& target) const noexcept {
	target.push_rectangle(rec, { 0.9, 0.1, 0.9, 1.0 });
	target.push_arrow(rec.center() - binding / 2, rec.center() + binding / 2, { 1, 1, 1, 1 });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		target.push_rectangle(
			rec,
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

Dispenser::Dispenser() noexcept {
	timer = (1 / hz) - std::fmodf(offset_timer, 1 / hz);
}

void Dispenser::render(render::Orders& target) const noexcept {
	target.push_circle(proj_r, start_pos, { 0.4, 0.3, 0.2, 1. });

	if (editor_selected) {
		target.push_circle(proj_r, start_pos, { 0.4, 0.3, 0.2, 1. }, 0.1f);

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

void Projectile::render(render::Orders& target) const noexcept {
	target.push_circle(r, pos, { 0.2, 0.3, 0.4, 1. });
}

void Decor_Sprite::render(render::Orders& target) const noexcept {
	target.push_sprite(rec, texture_key, {0, 0}, 0.f, { 1, 1, 1, opacity * 1. });

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f) * 0.5 + 0.25;

		target.push_rectangle(
			rec,
			{ 1, 1, 1, alpha * alpha },
			0.01f,
			{ 1.0, 0.0, 0.0, 1.0 }
		);
	}
}

void Key_Item::render(render::Orders& target) const noexcept {
	Vector4d color{ 1, 1, 1, 1 };
	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.0001f) * 0.5 + 0.25;

		color = { alpha, alpha, alpha, alpha };
	}

	target.push_sprite(pos, Key_World_Size, asset::Known_Textures::Key_Item, { 0, 0 }, 0, color);
}

void Level::render(render::Orders& target) const noexcept {
	auto renders = [&](const auto& cont) { for (const auto& x : cont) x.render(target); };

	renders(friction_zones);
	renders(auto_binding_zones);
	renders(trigger_zones);
	renders(next_zones);
	renders(dry_zones);
	renders(kill_zones);
	renders(doors);
	renders(blocks);
	renders(dispensers);

	renders(decor_sprites);

	renders(key_items);
	renders(prest_sources);
	renders(projectiles);
	renders(rocks);

	for (auto& x : markers) target.push_circle(0.05f, x, { 1, 0, 0, 1 });

	player.render(target);

	for (const auto& x : debug_vectors) target.push_arrow(x.a, x.a + x.b, { 1, 1, 0, 1 });
}

void Level::input(IM::Input_Iterator record) noexcept {
	mouse_screen_pos = record->mouse_screen_pos;
	mouse_world_pos = record->mouse_world_pos(game->camera);
	window_size = record->window_size;

	player.input(record);
	if (record->is_just_pressed(Keyboard::Return)) {
		markers.push_back(player.pos);

		if (record->is_pressed(Keyboard::LSHIFT)) markers.clear();
	}
}

void Level::update(float dt) noexcept {
	for (size_t i = 0; i < decor_sprites.size(); ++i) {
		auto& x = decor_sprites[i];
		if (!x.texture_loaded) {
			// >TODO
			(void)asset::Store.load_texture(x.texture_key, x.texture_path);
			x.texture_loaded = true;
		}
	}

	debug_vectors.clear();

	for (auto& x : trigger_zones) {
		x.triggered =
			test(x, { player.pos, player.size }) ||
			std::any_of(BEG_END(rocks), [&x](const Rock& y) { return test(y, x.rec); });
	}

	for (auto& x : doors) {
		bool open = true;
		
		for (auto& i : x.must_triggered) {
			open &= std::any_of(
				BEG_END(trigger_zones), [i](const auto& x) { return x.id == i && x.triggered; }
			);
		}
		for (auto& i : x.mustnt_triggered) {
			open &= std::any_of(
				BEG_END(trigger_zones), [i](const auto& x) { return x.id == i && !x.triggered; }
			);
		}
		open &= x.must_have_keys.empty();

		x.closed = !open;
	}

	for (auto& x : dispensers) {
		x.timer -= dt;
		if (x.timer <= 0) {
			x.timer = 1 / x.hz;

			Projectile p;
			p.pos = x.start_pos;
			p.r = x.proj_r;
			p.speed = (x.end_pos - x.start_pos).normalize() * x.proj_speed;
			p.end_pos = x.end_pos;

			projectiles.push_back(p);
		}
	}
	for (size_t i = projectiles.size() - 1; i + 1 > 0; --i) {
		auto& x = projectiles[i];

		x.pos += x.speed * dt;

		Circlef c;
		c.c = x.end_pos;
		c.r = x.r;

		if (is_in(x.pos, c)) {
			projectiles.erase(BEG(projectiles) + i);
		}
	}

	auto previous_player_pos = player.pos;
	player.update(dt);

	for (auto& d : doors) if (distTo2(player.pos, d.rec) < 1) match_and_destroy_keys(player, d);

	test_collisions(dt, previous_player_pos);
}

void Level::test_collisions(float dt, Vector2f previous_player_pos) noexcept {
	const auto dead_velocity_2 = Environment.dead_velocity * Environment.dead_velocity;

	player.colliding_blocks.clear();

	auto test_solids = [&](bool auto_climb) {
		auto_climb &= player.velocity.y < 0.1f;

		size_t correcting_count = 1;
		Vector2f current_old_player_pos = previous_player_pos;
	corrected_position:
		bool flag = false;
		for (size_t i = 0; i < doors.size(); ++i) {
			if (test(doors[i], player)) {
				if (auto_climb) {
					auto dt_y = player.pos.y - (blocks[i].pos.y + blocks[i].size.y);
					if (-player.size.y / 5.f < dt_y && dt_y < 0) {
						player.pos.y = blocks[i].pos.y + blocks[i].size.y;
						current_old_player_pos.y = player.pos.y;
						if (correcting_count-- > 0) goto corrected_position;
					}
				}

				flag = true;
			}
		}

		for (size_t i = 0; i < blocks.size(); ++i) {
			if (test(blocks[i], player)) {
				if (auto_climb) {
					auto dt_y = player.pos.y - (blocks[i].pos.y + blocks[i].size.y);
					if (-player.size.y / 5.f < dt_y && dt_y < 0) {
						player.pos.y = blocks[i].pos.y + blocks[i].size.y;
						current_old_player_pos.y = player.pos.y;
						if (correcting_count-- > 0) goto corrected_position;
					}
				}

				player.colliding_blocks.insert(i);
				if (blocks[i].kind == Block::Kind::Saturated) {
					player.saturated_touch_last_time = Player::Saturated_Touch_Last_Time;
				}
				flag = true;
			}
		}

		previous_player_pos = current_old_player_pos;
		return flag;
	};

	auto test_any = [](const auto& cont, const auto& to_test) {
		return std::any_of(BEG_END(cont), [&](auto x) {return test(x, to_test); });
	};
	auto test_any_p = [&p = player, test_any](const auto& cont) { return test_any(cont, p); };

	Vector2f flat_velocities = { 0, 0 };
	for (const auto& x : player.flat_velocities) flat_velocities += x;
	player.flat_velocities.clear();

	auto velocities = flat_velocities + player.velocity;
	for (const auto& x : friction_zones) {
		if (x.rec.intersect({ player.pos, player.size })) {
			velocities *= x.friction;
			player.velocity *= std::powf(x.friction, dt);
		}
	}
	float impact = 0;
	bool new_floored = false;

	player.pos.x += velocities.x * dt;
	if (test_solids(true)) {
		impact += velocities.x * velocities.x;
		player.pos.x = previous_player_pos.x;
		player.velocity.x = 0;
		player.forces.x = 0;
	}
	player.pos.y += velocities.y * dt;
	if (test_solids(false)) {
		impact += velocities.y * velocities.y;
		player.pos.y = previous_player_pos.y;
		new_floored = velocities.y < 0;
		player.velocity.y = 0;
		player.forces.y = 0;
	}

	if (player.floored && !new_floored && !player.just_jumped) {
		player.coyotee_timer = Player::Coyotee_Time;
	}
	if (!player.floored && new_floored && player.preshot_timer > 0) {
		player.jump();
	}
	else {
		player.just_jumped = false;
		player.floored = new_floored;
	}

	for (auto& rock : rocks) {
		const auto G = Environment.gravity;
		float length_sum = 0;
		for (const auto& x : rock.bindings) {
			rock.velocity += G * x * dt / rock.mass;
			length_sum += x.length();
		}
		rock.velocity.y -= (std::max(0.f, 1.f - length_sum) * Environment.gravity) * dt;

		Circlef circle;
		Rectanglef rec;

		circle.r = rock.r;
		circle.c = rock.pos + rock.velocity * dt;

		for (const auto& block : blocks) {
			rec.pos = block.pos;
			rec.size = block.size;

			if (auto opt = get_next_velocity(circle, rock.velocity, rec, dt); opt) {
				rock.velocity = *opt;
				circle.c = rock.pos + rock.velocity * dt;
			}
		}
		for (const auto& door : doors) {
			if (!door.closed) continue;
			if (auto opt = get_next_velocity(circle, rock.velocity, door.rec, dt); opt) {
				rock.velocity = *opt;
				circle.c = rock.pos + rock.velocity * dt;
			}
		}

		rock.pos = circle.c;
	}

	if (
		std::sqrtf(impact) > Environment.dead_velocity ||
		test_any_p(kill_zones) || test_any_p(projectiles)
	) {
		game->died = true;
	}

	for (const auto& d : dry_zones) {
		if (test(d, player)) player.clear_all_basic_bindings();
		for (auto& r : rocks) if (test(d, r)) r.bindings.clear();
	}

	for (auto& x : next_zones) {
		if (!test(x, player)) continue;

		game->next_level_path = Exe_Path / LEVEL_PATH / x.next_level;
		game->succeed = true;
	}

	for (auto& x : auto_binding_zones) {
		if (player.auto_binded_from_zones.count(x.uuid)) continue;

		if (
			x.rec.intersect({ player.pos, player.size }) &&
			!player.auto_binded_from_zones.contains(x.uuid)
		) {
			player.add_forced_binding(x.binding, x.uuid);
		}
	}

	for (size_t i = prest_sources.size() - 1; i + 1 > 0; --i) {
		if (test(prest_sources[i], player)) {
			player.prest += prest_sources[i].prest;
			prest_sources.erase(BEG(prest_sources) + i);
		}
	}

	for (size_t i = key_items.size() - 1; i + 1 > 0; --i) {
		if (test(key_items[i], { player.pos, player.size })) {
			player.own_keys.push_back(key_items[i].id);
			key_items.erase(BEG(key_items) + i);
		}
	}
}

void Level::pause() noexcept {}
void Level::resume() noexcept {
	auto iter = [](auto& x) noexcept { for (auto& y : x) y.editor_selected = false; };

	iter(rocks);
	iter(doors);
	iter(blocks);
	iter(key_items);
	iter(dry_zones);
	iter(next_zones);
	iter(kill_zones);
	iter(dispensers);
	iter(decor_sprites);
	iter(trigger_zones);
	iter(prest_sources);
	iter(friction_zones);
	iter(auto_binding_zones);
}

void Level::bind_rock(std::uint64_t x, Vector2f bind) noexcept {
	auto it = xstd::find_member(rocks, xstd::offset_of(&Rock::running_id), x);
	if (!it) return;

	it->bindings.push_back(bind);
}

void match_and_destroy_keys(Player& p, Door& d) noexcept {
	for (size_t i = p.own_keys.size() - 1; i + 1 > 0; --i) {
		for (size_t j = d.must_have_keys.size() - 1; j + 1 > 0; --j) {
			if (p.own_keys[i] == d.must_have_keys[j]) {
				p.own_keys.erase(BEG(p.own_keys) + i);
				d.must_have_keys.erase(BEG(d.must_have_keys) + j);
				goto parent_loop;
			}
		}
        parent_loop:;
	}
}

void from_dyn_struct(const dyn_struct& str, Block& block) noexcept {
	if (has(str, "kind")) block.kind = (Block::Kind)((int)str["kind"]);
	block.pos = (Vector2f)str["pos"];
	block.size = (Vector2f)str["size"];
}
void to_dyn_struct(dyn_struct& str, const Block& block) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = block.pos;
	str["size"] = block.size;
	str["kind"] = (int)block.kind;
}
void from_dyn_struct(const dyn_struct& str, Auto_Binding_Zone& zone) noexcept {
	zone.rec = (Rectanglef)str["rec"];
	zone.binding = (Vector2f)str["binding"];
	zone.uuid = (size_t)str["uuid"];
}
void to_dyn_struct(dyn_struct& str, const Auto_Binding_Zone& zone) noexcept {
	str = dyn_struct::structure_t{};
	str["rec"] = zone.rec;
	str["binding"] = zone.binding;
	str["uuid"] = zone.uuid;
}
void from_dyn_struct(const dyn_struct& str, Dry_Zone& x) noexcept {
	x.rec = (Rectanglef)str["rec"];
}
void to_dyn_struct(dyn_struct& str, const Dry_Zone& x) noexcept {
	str = dyn_struct::structure_t{};
	str["rec"] = x.rec;
}
void from_dyn_struct(const dyn_struct& str, Friction_Zone& x) noexcept {
	x.rec = (Rectanglef)str["rec"];
	x.friction = (float)str["friction"];
}
void to_dyn_struct(dyn_struct& str, const Key_Item& x) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = x.pos;
	str["id"] = x.id;
}
void from_dyn_struct(const dyn_struct& str, Key_Item& x) noexcept {
	x.pos = (Vector2f)str["pos"];
	x.id = (std::uint64_t)str["id"];
}
void to_dyn_struct(dyn_struct& str, const Friction_Zone& x) noexcept {
	str = dyn_struct::structure_t{};
	str["friction"] = x.friction;
	str["rec"] = x.rec;
}
void from_dyn_struct(const dyn_struct& str, Dispenser& dispenser) noexcept {
	if (has(str, "offset_timer")) dispenser.offset_timer = (float)str["offset_timer"];
	dispenser.start_pos = (Vector2f)str["start_pos"];
	dispenser.end_pos = (Vector2f)str["end_pos"];
	dispenser.proj_r = (float)str["proj_r"];
	dispenser.hz = (float)str["hz"];
	dispenser.proj_speed = (float)str["proj_speed"];
	dispenser.timer = (1 / dispenser.hz) - std::fmodf(dispenser.offset_timer, 1 / dispenser.hz);
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
#define X(x) if (has(str, #x)) level.x = (decltype(level.x))str[#x];
	X(blocks);
	X(friction_zones);
	X(dispensers);
	X(kill_zones);
	X(next_zones);
	X(prest_sources);
	X(key_items);
	X(decor_sprites);
	X(dry_zones);
	X(doors);
	X(trigger_zones);
	X(rocks);
	X(markers);
	X(auto_binding_zones);
	X(camera_bound);
#undef X
    
	Player player;
	player.forces = {};
	player.velocity = {};
	player.prest = (float)str["player"]["prest"];
	player.pos = (Vector2f)str["player"]["pos"];
	level.player = player;
    
	if (has(str["camera"], "pos")) {
		level.camera_start = {
			{0, 0},
			(Vector2f)str["camera"]["size"]
		};
		level.camera_start.setCenter((Vector2f)str["camera"]["pos"]);
	}
	else {
		level.camera_start = (Rectanglef)str["camera"];
	}
}
void to_dyn_struct(dyn_struct& str, const Level& level) noexcept {
	str = dyn_struct::structure_t{};
	str["blocks"] = level.blocks;
	str["kill_zones"] = level.kill_zones;
	str["key_items"] = level.key_items;
	str["next_zones"] = level.next_zones;
	str["dry_zones"] = level.dry_zones;
	str["prest_sources"] = level.prest_sources;
	str["dispensers"] = level.dispensers;
	str["friction_zones"] = level.friction_zones;
	str["decor_sprites"] = level.decor_sprites;
	str["markers"] = level.markers;
	str["rocks"] = level.rocks;
	str["doors"] = level.doors;
	str["auto_binding_zones"] = level.auto_binding_zones;
	str["trigger_zones"] = level.trigger_zones;
	str["camera_bound"] = level.camera_bound;
    
	auto& player = str["player"] = dyn_struct::structure_t{};
    
	player["prest"] = level.player.prest;
	player["pos"] = level.player.pos;
    
	str["camera"] = game->camera;
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
void from_dyn_struct(const dyn_struct& str, Trigger_Zone& x) noexcept {
	x.rec = (Rectanglef)str["rec"];
	x.id = (std::uint64_t)str["id"];
}
void to_dyn_struct(dyn_struct& str, const Trigger_Zone& x) noexcept {
	str = dyn_struct::structure_t{};
	str["id"] = x.id;
	str["rec"] = x.rec;
}
void from_dyn_struct(const dyn_struct& str, Door& x) noexcept {
	x.rec = (Rectanglef)str["rec"];
	x.closed = (bool)str["closed"];
	x.must_triggered = (decltype(x.must_triggered))str["must_triggered"];
	if (has(str, "must_have_keys"))
		x.must_have_keys = (decltype(x.must_have_keys))str["must_have_keys"];
	x.mustnt_triggered = (decltype(x.mustnt_triggered))str["mustnt_triggered"];
}
void to_dyn_struct(dyn_struct& str, const Door& x) noexcept {
	str = dyn_struct::structure_t{};
	str["rec"] = x.rec;
	str["closed"] = x.closed;
	str["must_triggered"] = x.must_triggered;
	str["must_have_keys"] = x.must_have_keys;
	str["mustnt_triggered"] = x.mustnt_triggered;
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
void from_dyn_struct(const dyn_struct& str, Decor_Sprite& x) noexcept {
	x.rec = (Rectanglef)str["rec"];
	x.texture_path = (std::string)str["texture_path"];

	auto& texture_loaded = asset::Store.textures_loaded;
	if (!texture_loaded.count(x.texture_path.string())) {
		x.texture_key = asset::Store.make_texture();
	}
	else {
		x.texture_key = std::find_if(
			BEG_END(texture_loaded), [p = x.texture_path.string()](auto x) {return x.first == p; }
		)->second;
	}
}
void to_dyn_struct(dyn_struct& str, const Decor_Sprite& x) noexcept {
	str = dyn_struct::structure_t{};
	str["rec"] = x.rec;
	str["texture_path"] = std::filesystem::canonical(x.texture_path).string();
}
