#include "Level.hpp"

#include <imgui.h>
#include <chrono>
#include <thread>
#include <functional>

#include "Math/Circle.hpp"
#include "Collision.hpp"
#include "Time.hpp"
#include "OS/file.hpp"
#include "Game.hpp"

#include "Assets.hpp"

void match_and_destroy_keys(Player& p, Door& d) noexcept;

void Block::render(render::Orders& target) const noexcept {
	Vector4d color = { 1, 1, 1, 1 };
	if (back) color = { .2, .8, .2, 1 };

	target.push_rectangle(pos, size, color);

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
	target.push_sprite(
		pos,
		{ 2 * r, 2 * r },
		asset::Texture_Id::Rock,
		{ 0, 0, 1, 1 },
		{ .5, .5 },
		0.f,
		{1, 1, 1, 1}
	);

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
	Vector4d color = { .9, .9, .9, 1. };
	if (!closed) color.a = .1;
	target.push_rectangle(rec, color);

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
	target.push_sprite(
		rec, texture_key, { 0, 0, 1, 1 }, { 0, 0 }, 0.f, { 1, 1, 1, opacity * 1. }
	);

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

	target.push_sprite(
		pos,
		Key_World_Size,
		asset::Texture_Id::Key_Item,
		{ 0, 0, 1, 1 },
		{ 0, 0 },
		0,
		color
	);
}

void Torch::render(render::Orders& target) const noexcept {
	Vector2f rand_pos;
	Vector4d rand_color;
	float rand_intensity;

#define rand_unit (rand() / (float)RAND_MAX)
#define PI 3.1415926

	double angles[] = {
		2 * PI * rand_unit, 2 * PI * rand_unit, 2 * PI * rand_unit
	};

	rand_pos = pos + random_factor * rand_unit * Vector2f::createUnitVector(2 * PI * rand_unit);
	rand_color = color + random_factor * rand_unit * Vector4d::createUnitVector(angles);
	rand_intensity = intensity + random_factor * rand_unit;

	target.push_point_light(rand_pos, rand_color, rand_intensity);
	target.late_push_circle(std::sqrtf(rand_intensity) / 10, rand_pos, rand_color);
}

void Moving_Block::render(render::Orders& target) const noexcept {
	for (auto& x : waypoints) {
		target.push_circle(0.1f, x, { 0.1, 0.15, 0.5, 1.0 });
	}

	Vector4d color{ 0.2, 0.3, 1.0, 1.0 };
	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.0001f) * 0.5 + 0.25;

		color = { alpha, alpha, alpha, alpha };
	}
	target.push_rectangle(rec, color);
}

void Moving_Block::update(float dt) noexcept {
	auto compute_point_at_traj = [&] {
		if (waypoints.size() <= 1) return waypoints[0];
		if (t == max_t) return waypoints.back();

		float segment{ 0 };
		float local_t{ t };
		size_t i = 0;
		do {
			local_t -= segment;
			++i;

			segment = waypoints[i - 1].dist_to(waypoints[i]);
		} while (local_t > segment);

		return waypoints[i - 1] + local_t * (waypoints[i] - waypoints[i - 1]).normalize();
	};
	to_move = {};

	if (waypoints.empty()) return;

	if (reverse) {
		t -= speed * dt;
		if (t < 0) {
			colliding_player = false;
			if (looping) {
				t = -t;
				reverse = false;
			}
			else {
				t += max_t;
			}
		}
	}
	else {
		t += speed * dt;
		if (t > max_t) {
			colliding_player = false;
			if (looping) {
				t = 2 * max_t - t;
				reverse = true;
			}
			else {
				t -= max_t;
			}
		}
	}

	to_move = compute_point_at_traj() - rec.center();
}

void Level::render(render::Orders& target) const noexcept {
	target.push_view(camera);

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
	renders(torches);

	renders(moving_blocks);

	renders(key_items);
	renders(prest_sources);
	renders(projectiles);
	renders(rocks);

	for (auto& x : markers) target.push_circle(0.05f, x, { 1, 0, 0, 1 });

	player.render(target);

	for (const auto& x : debug_vectors) target.push_arrow(x.a, x.a + x.b, { 1, 1, 0, 1 });

	target.push_ambient_light(ambient_color, ambient_intensity);

	if (focused_rock) {
		target.push_sprite(
			rocks[*focused_rock].pos,
			V2F(1.5f),
			asset::Texture_Id::Indicator,
			{ 0, 0, 1, 1 },
			{.5f , .5f}
		);
	}
}

void Level::input(IM::Input_Iterator record) noexcept {
	mouse_screen_pos = record->mouse_screen_pos;
	mouse_world_pos = record->mouse_world_pos(camera);
	window_size = record->window_size;

	player.input(record);
	if (record->is_just_pressed(Keyboard::Return)) {
		markers.push_back(player.pos);

		if (record->is_pressed(Keyboard::LSHIFT)) markers.clear();
	}

	auto range = Environment.binding_range * Environment.binding_range;
	if (record->is_just_pressed(Joystick::LB) && !rocks.empty()) {
		auto sorted_rocks = rocks;
		sorted_rocks.erase(std::remove_if(BEG_END(sorted_rocks), [&](Rock& x) {
			return (x.pos - player.pos).length2() > range;
		}), END(sorted_rocks));
		std::sort(BEG_END(sorted_rocks), [&](const Rock& a, const Rock& b) {
			return (a.pos - player.pos).length() < (b.pos - player.pos).length();
		});

		if (!sorted_rocks.empty() && !focused_rock) {
			focused_rock =
				std::distance(BEG(rocks), std::find(BEG_END(rocks), *BEG(sorted_rocks)));
		}
		else if (!sorted_rocks.empty()) {
			auto it = std::find(BEG_END(sorted_rocks), rocks[*focused_rock]);
			if (it == sorted_rocks.end() - 1) {
				focused_rock.reset();
			}
			else {
				focused_rock = std::distance(BEG(rocks), std::find(BEG_END(rocks), *(it + 1)));
			}
		}
	}
}

void Level::update(float dt) noexcept {
	update_camera(dt);

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

	for (auto& x : moving_blocks) {
		x.update(dt);
		x.rec.pos += x.to_move;

		if (test(player, x.rec)) x.colliding_player = true;
		if (x.colliding_player) player.pos += x.to_move;
	}

	if (focused_rock) {
		auto range = Environment.binding_range * Environment.binding_range;

		if (player.pos.dist_to2(rocks[*focused_rock].pos) > range) focused_rock.reset();
	}

	update_player(dt);
	test_collisions(dt);

	for (auto& d : doors) if (dist_to2(player.pos, d.rec) < 1) match_and_destroy_keys(player, d);
}

void Level::test_collisions(float dt) noexcept {
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

			if (block.back) continue;

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

void Level::update_player(float dt) noexcept {
	auto previous_player_pos = player.pos;
	player.update(dt);

	bool just_direct_control_velocity{ false };
	bool hard_border{ false };

	auto test_solids = [&](bool auto_climb) {
		auto_climb &= player.velocity.y < 0.1f;

		size_t correcting_count = 1;
	corrected_position:

		bool flag = false;
		for (size_t i = 0; i < doors.size(); ++i) {
			if (!test(doors[i], player)) continue;
			flag = true;

			if (!auto_climb) continue;
			
			auto dt_y = player.pos.y - (blocks[i].pos.y + blocks[i].size.y);
			if (-player.size.y / 5.f < dt_y && dt_y < 0) {
				player.pos.y = blocks[i].pos.y + blocks[i].size.y;
				previous_player_pos.y = player.pos.y;
				if (correcting_count-- > 0) goto corrected_position;
			}
		}

		for (size_t i = 0; i < blocks.size(); ++i) {
			auto& b = blocks[i];

			auto grap_box = Rectanglef{ player.pos, player.size };
			grap_box.size += .1f;
			grap_box.pos -= .05f;

			if (player.grappling && test(b, grap_box)) {

				player.grappled = true;
				just_direct_control_velocity = true;
				player.clear_movement_x();
				player.clear_movement_y();

				player.grappling_normal = Rectanglef{ b.pos, b.size }.get_normal_to(player.pos);
			}

			if (!test(b, player)) continue;

			hard_border = !b.back;
			flag = true;


			if (b.prest_kind == Block::Prest_Kind::Saturated) {
				player.saturated_touch_last_time = Player::Saturated_Touch_Last_Time;
			}

			if (!auto_climb) continue;

			auto dt_y = player.pos.y - (b.pos.y + b.size.y);
			if (-player.size.y / 5.f < dt_y && dt_y < 0) {
				player.pos.y = b.pos.y + b.size.y;
				previous_player_pos.y = player.pos.y;
				if (correcting_count-- > 0) goto corrected_position;
			}
		}

		for (size_t i = 0; i < moving_blocks.size(); ++i) {
			moving_blocks[i].colliding_player = false;
			if (!test(player, moving_blocks[i].rec)) continue;

			moving_blocks[i].colliding_player |=
				player.pos.y >= moving_blocks[i].rec.y + moving_blocks[i].rec.h;
			flag = true;
		}



		return flag;
	};

	auto velocities = player.get_final_velocity();
	auto direct_velocities = player.get_direct_control_velocity();
	player.flat_velocities.clear();
	for (const auto& x : friction_zones) {
		if (x.rec.intersect({ player.pos, player.size })) {
			velocities *= x.friction;
			player.apply_friction(std::powf(x.friction, dt));
		}
	}
	float impact = 0;
	bool new_floored = false;

	bool collided_x{ false };
	bool collided_y{ false };

	bool hard_border_x{ false };
	bool hard_border_y{ false };

	player.pos.x += velocities.x * dt;
	collided_x = test_solids(true);
	hard_border_x = hard_border;
	if (collided_x) {
		impact += velocities.x * velocities.x;
		player.pos.x = previous_player_pos.x;
		player.clear_movement_x();
	}
	player.pos.y += velocities.y * dt;
	collided_y = test_solids(false);
	hard_border_y = hard_border;
	if (collided_y) {
		impact += velocities.y * velocities.y;
		player.pos.y = previous_player_pos.y;
		new_floored = velocities.y < 0;
		player.clear_movement_y();
	}

	if (just_direct_control_velocity) {
		if (collided_x && !hard_border_x) player.pos.x += direct_velocities.x * dt * .1f;
		if (collided_y && !hard_border_y) player.pos.y += direct_velocities.y * dt * .1f;
	}

	if (player.floored && !new_floored && !player.just_jumped) {
		player.coyotee_timer = Player::Coyotee_Time;
	}
	if (!player.floored && new_floored && player.preshot_timer > 0) {
		player.jump();
	}
	else {
		player.just_jumped = false;
		if (new_floored && !player.floored) player.just_floored = true;
		player.floored = new_floored;
	}


	if (!game->died) {
		for (auto& x : kill_zones) if (test(x, player)) {
			game->died |= true;
			break;
		}
		for (auto& x : projectiles) if (test(x, player)) {
			game->died |= true;
			break;
		}
		game->died |= std::sqrtf(impact) > Environment.dead_velocity;
	}
}


void Level::update_camera(float dt) noexcept {
	auto camera_target = player.pos;
	auto camera_center = camera.center();
	auto dist = (camera_target - camera_center).length();

	if (dist > camera_idle_radius) {
		Vector2f dt_pos = camera_target - camera_center;
		Vector2f to_move =
			std::min(dist - camera_idle_radius, dt * camera_speed) * dt_pos.normalize();
		camera.pos += to_move;
	}

	if (camera_bound.area() > 0) camera = camera.restrict_in(camera_bound);

}

void Level::pause() noexcept {}
void Level::resume() noexcept {
	auto iter = [](auto& x) noexcept { for (auto& y : x) y.editor_selected = false; };

	iter(rocks);
	iter(doors);
	iter(blocks);
	iter(torches);
	iter(key_items);
	iter(dry_zones);
	iter(next_zones);
	iter(kill_zones);
	iter(dispensers);
	iter(moving_blocks);
	iter(decor_sprites);
	iter(trigger_zones);
	iter(prest_sources);
	iter(friction_zones);
	iter(auto_binding_zones);
}

void Level::bind_rock(std::uint64_t x, Vector2f bind) noexcept {
	auto it = xstd::find_member(rocks, xstd::offset_of(&Rock::running_id), x);
	assert(it);

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

void from_dyn_struct(const dyn_struct& str, Moving_Block& block) noexcept {
	block.looping   = (bool)                 str["looping"];
	block.max_t     = (float)                str["max_t"];
	block.rec       = (Rectanglef)           str["rec"];
	block.reverse   = (bool)                 str["reverse"];
	block.speed     = (float)                str["speed"];
	block.t         = (float)                str["t"];
	block.waypoints = (std::vector<Vector2f>)str["waypoints"];
}
void to_dyn_struct(dyn_struct& str, const Moving_Block& block) noexcept {
	str = dyn_struct::structure_t{};
	str["looping"]   = block.looping;
	str["max_t"]     = block.max_t;
	str["rec"]       = block.rec;
	str["reverse"]   = block.reverse;
	str["speed"]     = block.speed;
	str["t"]         = block.t;
	str["waypoints"] = block.waypoints;
}
void from_dyn_struct(const dyn_struct& str, Block& block) noexcept {
	if (has(str, "kind")) block.prest_kind = (Block::Prest_Kind)((int)str["kind"]);
	if (has(str, "back")) block.back = (bool)str["back"];
	block.pos = (Vector2f)str["pos"];
	block.size = (Vector2f)str["size"];
}
void to_dyn_struct(dyn_struct& str, const Block& block) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = block.pos;
	str["size"] = block.size;
	str["kind"] = (int)block.prest_kind;
	str["back"] = block.back;

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
void from_dyn_struct(const dyn_struct& str, Torch& x) noexcept {
	x.color = (Vector4d)str["color"];
	x.pos = (Vector2f)str["pos"];
	x.intensity = (float)str["intensity"];
	if (has(str, "random_factor")) x.random_factor = (float)str["random_factor"];
}
void to_dyn_struct(dyn_struct& str, const Torch& x) noexcept {
	str = dyn_struct::structure_t{};
	str["color"] = x.color;
	str["pos"] = x.pos;
	str["intensity"] = x.intensity;
	str["random_factor"] = x.random_factor;
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
	X(moving_blocks)
	X(doors);
	X(trigger_zones);
	X(rocks);
	X(markers);
	X(auto_binding_zones);
	X(torches);
	X(camera_bound);
#undef X
    
	Player player;
	player.forces = {};
	player.velocity = {};
	player.prest = (float)str["player"]["prest"];
	player.pos = (Vector2f)str["player"]["pos"];
	level.player = player;
    
	if (has(str, "ambient")) {
		level.ambient_intensity = (float)(str["ambient"]["intensity"]);
		level.ambient_color = (Vector4d)(str["ambient"]["color"]);
	}

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

	level.camera = level.camera_start;
}
void to_dyn_struct(dyn_struct& str, const Level& level) noexcept {
	str = dyn_struct::structure_t{};
	str["blocks"]             = level.blocks;
	str["kill_zones"]         = level.kill_zones;
	str["key_items"]          = level.key_items;
	str["next_zones"]         = level.next_zones;
	str["dry_zones"]          = level.dry_zones;
	str["prest_sources"]      = level.prest_sources;
	str["dispensers"]         = level.dispensers;
	str["friction_zones"]     = level.friction_zones;
	str["decor_sprites"]      = level.decor_sprites;
	str["markers"]            = level.markers;
	str["rocks"]              = level.rocks;
	str["doors"]              = level.doors;
	str["auto_binding_zones"] = level.auto_binding_zones;
	str["trigger_zones"]      = level.trigger_zones;
	str["camera_bound"]       = level.camera_bound;
	str["torches"]            = level.torches;
	str["moving_blocks"]      = level.moving_blocks;

	auto& player = str["player"] = dyn_struct::structure_t{};
    
	player["prest"] = level.player.prest;
	player["pos"] = level.player.pos;
    
	str["ambient"] = {
		{"color", level.ambient_color},
		{"intensity", level.ambient_intensity}
	};

	str["camera"] = level.camera;
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
