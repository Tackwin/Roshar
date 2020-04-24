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

#include "Math/Random.hpp"

void match_and_destroy_keys(Player& p, Door& d) noexcept;

void Flowing_Water::render(render::Orders& target) const noexcept {
	for (auto& x : path) {
		target.push_circle(0.1f, x, { 0.1, 0.15, 0.5, 1.0 });
	}
}

void Block::render(render::Orders& target) const noexcept {
	Vector4d color = { 1, 1, 1, 1 };
	if (back) color = { .2, .8, .2, 1 };
	if (destroy_on_step) color.a = std::clamp(destroy_timer / destroy_time, 0.f, 1.f);

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

	double angles[] = {
		2 * PI * randomf(), 2 * PI * randomf(), 2 * PI * randomf()
	};

	rand_pos = pos + random_factor * randomf() * Vector2f::createUnitVector(2 * PI * randomf());
	rand_color = color + random_factor * randomf() * Vector4d::createUnitVector(angles);
	rand_intensity = intensity + random_factor * randomf();

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
			if (looping) {
				t = -t;
				reverse = false;
			}
			else {
				moving_player = false;
				t += max_t;
			}
		}
	}
	else {
		t += speed * dt;
		if (t > max_t) {
			if (looping) {
				t = 2 * max_t - t;
				reverse = true;
			}
			else {
				moving_player = false;
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

	renders(flowing_waters);

	for (auto& x : markers) target.push_circle(0.05f, x, { 1, 0, 0, 1 });

	player.render(target);
	for (auto& p : phantom_paths) {
		if (phantom_path_idx >= p.size()) continue;

		thread_local Player phantom_player;
		phantom_player.render(target, p[phantom_path_idx]);
	}

	for (const auto& x : debug_vectors) target.push_arrow(x.a, x.a + x.b, { 1, 1, 0, 1 });

	for (auto& x : particles) x.render(target);

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
	if (Environment.show_camera_target) {
		target.push_circle(0.05f, camera_target, {1, 0, 0, 1});
		target.push_circle(0.025f, camera_shake_target, {1, 1, 1, 1});
	}

	target.late_push_view({ { 0, 0 }, { 1, Environment.window_ratio } });
	defer { target.late_pop_view(); };

	char buffer[100];

	sprintf(buffer, "%7.2f", score_timer);

	if (game->play_screen.in_replay) {
		char max_score_buffer[512];
		sprintf(
			max_score_buffer,
			"%7.2f/%7.2f",
			game->profile->best_time[name].last,
			game->profile->best_time[name].best
		);
		
		target.late_push_text(
			{ 0.5f , Environment.window_ratio - 0.01f },
			asset::Font_Id::Consolas,
			std::string(max_score_buffer),
			0.03f,
			{ 0.5, 1 }
		);
	}
	target.late_push_text(
		{ 0.005f , Environment.window_ratio - 0.005f },
		asset::Font_Id::Consolas,
		std::string(buffer),
		0.01f,
		{ 0, 1 }
	);
}

void Level::render_debug(render::Orders& target) const noexcept {
}

void Level::input(IM::Input_Iterator record) noexcept {
	mouse_screen_pos = record->mouse_screen_pos;
	mouse_world_pos = record->mouse_world_pos(camera);
	window_size = record->window_size;


	player.input(record);
	if (record->is_just_pressed(Keyboard::Return)) {
		markers.push_back(player.hitbox.center());

		if (record->is_pressed(Keyboard::LSHIFT)) markers.clear();
	}

	auto range = Environment.binding_range * Environment.binding_range;
	if (record->is_just_pressed(Joystick::LB) && !rocks.empty()) {
		auto sorted_rocks = rocks;
		sorted_rocks.erase(std::remove_if(BEG_END(sorted_rocks), [&](Rock& x) {
			return (x.pos - player.hitbox.pos).length2() > range;
		}), END(sorted_rocks));
		std::sort(BEG_END(sorted_rocks), [&](const Rock& a, const Rock& b) {
			return (a.pos - player.hitbox.pos).length() < (b.pos - player.hitbox.pos).length();
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
	score_timer += dt;

	update_camera(dt);

	phantom_path_idx++;

	for (auto& x : particle_spots) if (x.is_valid()) {
		x.update(dt, particles);
	}
	particle_spots.erase(
		std::remove_if(BEG_END(particle_spots), [](auto& p) { return p.timers.empty(); }),
		END(particle_spots)
	);

	for (auto& x : particles) x.update(dt);
	particles.erase(
		std::remove_if(BEG_END(particles), [](auto& x) { return x.t <= 0; }),
		END(particles)
	);

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
			test(x, player.hitbox) ||
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

	for (size_t i = blocks.size() - 1; i + 1 > 0; --i) {
		auto& x = blocks[i];
		if (!x.stepped_on) continue;

		x.destroy_timer -= dt;
		if (x.destroy_on_step && x.destroy_timer < 0.f) blocks.erase(BEG(blocks) + i);
	}

	for (auto& x : moving_blocks) {
		x.update(dt);
		x.rec.pos += x.to_move;

		// After moving it might be the first time that we collide with the player.
		if (test(x, player.hitbox)) x.moving_player = true;
		if (x.moving_player) player.hitbox.pos += x.to_move;
	}

	if (focused_rock) {
		auto range = Environment.binding_range * Environment.binding_range;

		if (player.hitbox.center().dist_to2(rocks[*focused_rock].pos) > range)
			focused_rock.reset();
	}

	update_player(dt);

	shake_factor = player.get_final_velocity().length() / Environment.dead_velocity;
	shake_factor = shake_factor * shake_factor - shake_treshold;

	test_collisions(dt);

	for (auto& d : doors) if (dist_to2(player.hitbox.center(), d.rec) < 1)
		match_and_destroy_keys(player, d);

	if (player.grappled){
		debug_vectors.push_back({
			player.hitbox.center(), player.grappling_normal
		});
	}

	ImGui::Text("Shake_factor: %f", shake_factor);
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

		game->play_screen.next_level_path = Exe_Path / LEVEL_PATH / x.next_level;
		game->play_screen.succeed = true;
		game->play_screen.new_time(score_timer);
	}

	for (auto& x : auto_binding_zones) {
		if (player.auto_binded_from_zones.count(x.uuid)) continue;

		if (
			x.rec.intersect(player.hitbox) &&
			player.auto_binded_from_zones.count(x.uuid) == 0
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
		if (test(key_items[i], player.hitbox)) {
			player.own_keys.push_back(key_items[i].id);
			key_items.erase(BEG(key_items) + i);
		}
	}
}

void Level::update_player(float dt) noexcept {
	auto previous_player_pos = player.hitbox.pos;
	player.update(dt);

	bool just_direct_control_velocity{ false };
	bool hard_border{ false };

	struct Player_Response {
		struct Holding_Block {
			bool back;
			Rectanglef rec;
		};

		bool collided{ false };
		float auto_climbed{ 0.f };
		bool touched_saturated{ false };
		std::optional<Holding_Block> holded{ std::nullopt };
		Moving_Block* moving_block_collided{ nullptr };
	};

	struct Player_Capabilities {
		bool auto_climb{ false };
		bool holding{ false };
	};

	auto simulate = [&](Player_Capabilities capabilities) -> Player_Response {
		Player_Response response = {};
		
		auto grap_box = player.hitbox;
		grap_box.size += .1f;
		grap_box.pos -= .05f;

		for (auto& x : doors) if (test(x, player)) response.collided = true;
		for (auto& x : moving_blocks) {
			x.moving_player = false;
			
			if (capabilities.holding && x.rec.intersect(grap_box)){
				response.holded = Player_Response::Holding_Block{};
				response.holded->back = false;
				response.holded->rec = x.rec;
				x.moving_player = true;
			}

			if (!test(player, x.rec)) continue;


			x.moving_player = true;
			response.collided = true;
		}

		for (auto& x : blocks) {
			if (capabilities.holding && test(x, grap_box)){
				response.holded = Player_Response::Holding_Block{};
				response.holded->back = x.back;
				response.holded->rec = {x.pos, x.size};
			}

			if (!test(x, player)) continue;

			x.stepped_on = true;
			response.touched_saturated |= x.prest_kind == Block::Prest_Kind::Saturated;

			auto dt_y = player.hitbox.y - (x.pos.y + x.size.y);
			bool can_climb = capabilities.auto_climb && -player.hitbox.h / 5.f < dt_y && dt_y < 0;

			if (can_climb) response.auto_climbed = dt_y;
			else           response.collided |= !x.back;
		}

		return response;
	};

	auto velocities = player.get_final_velocity();
	auto direct_velocities = player.get_direct_control_velocity();
	player.flat_velocities.clear();

	for (const auto& x : friction_zones) if (x.rec.intersect(player.hitbox)) {
		velocities *= x.friction;
		player.apply_friction(std::powf(x.friction, dt));
	}

	float impact = 0;

	bool new_floored = false;
	bool touched_saturated = false;
	std::optional<Player_Response::Holding_Block> grappled = std::nullopt;



	Player_Capabilities capabilities = {};
	Player_Response     response_x = {};
	Player_Response     response_y = {};

	player.hitbox.x += velocities.x * dt;
	capabilities.auto_climb = true;
	capabilities.holding = player.grappling;
	response_x = simulate(capabilities);

	touched_saturated |= response_x.touched_saturated;
	grappled = response_x.holded;

	if (response_x.auto_climbed) {
		// if we detected an opportunity to auto climb
		// we re-run the simulation with the new pos.

		capabilities.auto_climb = false; // making sure that we don't climb to infinities.
		player.hitbox.y -= 1.1f * response_x.auto_climbed;
		response_x = simulate(capabilities);
	}

	if (response_x.collided) {
		impact += velocities.x * velocities.x;
		player.hitbox.x = previous_player_pos.x;
		player.clear_movement_x();
	}

	capabilities.auto_climb = false;
	player.hitbox.y += velocities.y * dt;
	response_y = simulate(capabilities);

	touched_saturated |= response_y.touched_saturated;
	grappled = response_y.holded ? response_y.holded : grappled;

	if (response_y.collided) {
		impact += velocities.y * velocities.y;
		player.hitbox.y = previous_player_pos.y;
		player.clear_movement_y();
		new_floored = velocities.y < 0;
	}




	if (touched_saturated) player.saturated_touch_last_time = Player::Saturated_Touch_Last_Time;
	if (grappled) {
		player.grappled = true;
		just_direct_control_velocity = true;
		player.clear_movement_x();
		player.clear_movement_y();
		
		if (!response_x.collided) player.hitbox.x += direct_velocities.x * dt * .1f;
		if (!response_y.collided) player.hitbox.y += direct_velocities.y * dt * .1f;

		if (grappled->back) {
			player.grappling_normal = { 0, 1 };
		}
		else {
			Rectanglef rec = { grappled->rec.pos, grappled->rec.size };
			player.grappling_normal = rec.get_normal_to(player.hitbox);
		}
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


	if (!game->play_screen.died) {
		for (auto& x : kill_zones) if (test(x, player)) {
			game->play_screen.died |= true;
			break;
		}
		for (auto& x : projectiles) if (test(x, player)) {
			game->play_screen.died |= true;
			break;
		}
		game->play_screen.died |= std::sqrtf(impact) > Environment.dead_velocity;
	}
}


void Level::update_camera(float dt) noexcept {

	bool shaking = shake_factor > 0;
	camera_target = player.hitbox.center();
	
	
	if (shaking && !unit_shake) {
		unit_shake = Vector2f::rand_unit([]() -> double { return randomf(); });
	}
	if (unit_shake) {
		auto mult = shake_factor * camera_idle_radius * (randomf() * 1.5 - .5);
		camera_shake_target = camera_target + *unit_shake * mult;
	}

	auto camera_center = camera.center();
	auto dist = (camera_target - camera_center).length();
	auto dist_shake = (camera_shake_target - camera_target).length();

	if (dist > camera_idle_radius) {
		auto speed = (shaking ? shake_factor : 1) * dt * camera_speed;
		Vector2f dt_pos = camera_target - camera_center;
		Vector2f to_move =
			std::min(dist - camera_idle_radius, dt * camera_speed) * dt_pos.normalize();

		camera.pos += to_move;
	}

	if (shaking && dist_shake > camera_shake_idle_radius) {
		Vector2f dt_pos = camera_shake_target - camera_target;
		Vector2f to_move = std::min(
			dist - camera_shake_idle_radius, dt * camera_speed * shake_factor
		) * dt_pos.normalize();

		camera.pos += to_move;
		
	} else {
		unit_shake.reset();
	}
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
	iter(flowing_waters);
	iter(auto_binding_zones);
}

void Level::bind_rock(std::uint64_t x, Vector2f bind) noexcept {
	auto it = xstd::find_member(rocks, xstd::offset_of(&Rock::running_id), x);
	assert(it);

	it->bindings.push_back(bind);
}

void Level::feed_phantom_path(std::vector<std::vector<Player::Graphic_State>> p) noexcept {
	phantom_paths = std::move(p);
	phantom_path_idx = 0;
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

void from_dyn_struct(const dyn_struct& str, Flowing_Water& water) noexcept {
	water.path      = (std::vector<Vector2f>)str["path"];
	water.flow_rate = (float)str["flow_rate"];
	water.width     = (float)str["width"];
}

void to_dyn_struct(dyn_struct& str, const Flowing_Water& water) noexcept {
	str = dyn_struct::structure_t{};
	str["path"] = water.path;
	str["flow_rate"] = water.flow_rate;
	str["width"] = water.width;
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
	if (has(str, "destroy_on_step")) {
		block.destroy_on_step = (bool)str["destroy_on_step"];
		if (has(str, "destroy_time")) {
			block.destroy_time = (float)str["destroy_time"];
			block.destroy_timer = block.destroy_time;
		}
	}
	block.pos = (Vector2f)str["pos"];
	block.size = (Vector2f)str["size"];
}
void to_dyn_struct(dyn_struct& str, const Block& block) noexcept {
	str = dyn_struct::structure_t{};
	str["pos"] = block.pos;
	str["size"] = block.size;
	str["kind"] = (int)block.prest_kind;
	str["back"] = block.back;
	str["destroy_on_step"] = block.destroy_on_step;
	str["destroy_time"] = block.destroy_time;
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
	X(flowing_waters);
	X(torches);
#undef X

	Player player;
	player.forces = {};
	player.velocity = {};
	player.prest = (float)str["player"]["prest"];
	player.hitbox.pos = (Vector2f)str["player"]["pos"];
	level.player = player;

	level.ambient_intensity = (float)(str["ambient"]["intensity"]);
	level.ambient_color = (Vector4d)(str["ambient"]["color"]);
	level.camera = (Rectanglef)str["camera"];

	if (has(str, "name")) level.name = (std::string)str["name"];
}
void to_dyn_struct(dyn_struct& str, const Level& level) noexcept {
	str = dyn_struct::structure_t{};
	str["blocks"]              = level.blocks;
	str["kill_zones"]          = level.kill_zones;
	str["key_items"]           = level.key_items;
	str["next_zones"]          = level.next_zones;
	str["dry_zones"]           = level.dry_zones;
	str["prest_sources"]       = level.prest_sources;
	str["dispensers"]          = level.dispensers;
	str["friction_zones"]      = level.friction_zones;
	str["decor_sprites"]       = level.decor_sprites;
	str["markers"]             = level.markers;
	str["rocks"]               = level.rocks;
	str["doors"]               = level.doors;
	str["auto_binding_zones"]  = level.auto_binding_zones;
	str["trigger_zones"]       = level.trigger_zones;
	str["torches"]             = level.torches;
	str["moving_blocks"]       = level.moving_blocks;
	str["flowing_waters"]      = level.flowing_waters;

	auto& player = str["player"] = dyn_struct::structure_t{};
    
	player["prest"] = level.player.prest;
	player["pos"] = level.player.hitbox.pos;
    
	str["ambient"] = {
		{"color", level.ambient_color},
		{"intensity", level.ambient_intensity}
	};

	str["camera"] = level.camera;
	str["name"] = level.name.empty() ? level.file_path.generic_string() : level.name;
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
	auto relative_path = std::filesystem::weakly_canonical(x.texture_path);
	relative_path = relative_path.lexically_relative(Exe_Path);

	str = dyn_struct::structure_t{};
	str["rec"] = x.rec;
	str["texture_path"] = relative_path.generic_string();
}
