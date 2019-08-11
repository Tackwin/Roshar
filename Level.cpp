#include "Level.hpp"

#include <imgui.h>
#include <chrono>
#include <thread>

#include "Math/Circle.hpp"
#include "Collision.hpp"
#include "Time.hpp"
#include "OS/file.hpp"

Level_Store_t Level_Store;

void Block::render(sf::RenderTarget& target) const noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
	shape.setOutlineThickness(0.00f);
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

void Kill_Zone::render(sf::RenderTarget& target) const noexcept {
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

void Next_Zone::render(sf::RenderTarget& target) const noexcept {
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

void Dry_Zone::render(sf::RenderTarget& target) const noexcept {
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

void Rock::render(sf::RenderTarget& target) const noexcept {
	sf::CircleShape shape(r, 1000);
	shape.setOrigin(r, r);
	shape.setPosition(pos);
	shape.setFillColor(Vector4d{ 0.5, 0.6, 0.7, 1.0 });
	target.draw(shape);

	for (auto& binding : bindings) {
		Vector2f::renderArrow(
			target,
			pos,
			pos + binding,
			{ 1, 0, 1, 1 },
			{ 1, 0, 1, 1 }
		);
	}

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		shape.setRadius(r + 0.04f);
		shape.setOrigin(shape.getRadius(), shape.getRadius());
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, alpha * alpha });
		target.draw(shape);
	}
}

void Trigger_Zone::render(sf::RenderTarget& target) const noexcept {
	sf::RectangleShape shape(rec.size);
	shape.setPosition(rec.pos);
	shape.setFillColor(sf::Color::Magenta);
	target.draw(shape);

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		shape.setSize(rec.size);
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(.01f);
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, alpha * alpha });
		shape.setPosition(rec.pos);
		target.draw(shape);
	}
}

void Door::render(sf::RenderTarget& target) const noexcept {
	sf::RectangleShape shape(rec.size);
	shape.setPosition(rec.pos);
	shape.setFillColor(Vector4d{ 0.9, 0.9, 0.9, closed ? 1.0 : 0.1});
	target.draw(shape);

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f);

		shape.setSize(rec.size);
		shape.setOutlineColor(Vector4f{ 1.f, 0.f, 0.f, 1.f });
		shape.setOutlineThickness(.01f);
		shape.setFillColor(Vector4f{ 1.f, 1.f, 1.f, alpha * alpha });
		shape.setPosition(rec.pos);
		target.draw(shape);
	}
}

void Prest_Source::render(sf::RenderTarget& target) const noexcept {
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

void Player::update(float dt) noexcept {
	coyotee_timer -= dt;
	preshot_timer -= dt;
	speed_up_timer -= dt;
	speed_down_timer -= dt;
	jump_strength_modifier_timer -= dt;

	if (last_dir != None && speed_down_timer > 0) {
		auto slow_down = std::clamp(speed_down_timer / Speed_Down_Time, 0.f, 1.f);
		if (last_dir == Right) slow_down *= -5;
		else slow_down *= 5;

		flat_velocities.push_back({ slow_down, 0 });
	}
}

void Player::render(sf::RenderTarget& target) const noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.0, 0.0, 0.0, 1.0 });
	shape.setOutlineThickness(0.01f);
	shape.setPosition(pos);
	shape.setFillColor(sf::Color::Cyan);
	target.draw(shape);
}

void Player::start_move_sideway() noexcept {
	if (speed_up_timer <= 0) speed_up_timer = Speed_Up_Time;
}
void Player::stop_move_sideway() noexcept {
	speed_down_timer = Speed_Down_Time;
}
void Player::move_sideway(Player::Dir dir) noexcept {
	auto top_speed = std::sqrtf(std::clamp(1.f - speed_up_timer / Speed_Up_Time, 0.f, 1.f));
	if (dir == Right) top_speed *= -5;
	else if (dir == Left) top_speed *= 5;
	else assert(false); // Logic error.
	flat_velocities.push_back({ top_speed, 0 });
	last_dir = dir;
}

void Player::jump() noexcept {
	velocity += Vector2f{ 0, 6.5 };
	jump_strength_modifier_timer = Jump_Strength_Modifier_Time;
}
void Player::maintain_jump() noexcept {
	flat_velocities.push_back({ 0, 1.5 });
}
void Player::directional_up() noexcept {
	flat_velocities.push_back({ 0, 0.75 });
}

void Dispenser::set_start_timer() noexcept {
	timer = (1 / hz) - std::fmodf(offset_timer, 1 / hz);
}

void Dispenser::render(sf::RenderTarget& target) const noexcept {
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

void Projectile::render(sf::RenderTarget& target) const noexcept {
	sf::CircleShape shape(r);
	shape.setFillColor(Vector4f{ 0.2f, 0.3f, 0.4f, 1.f });
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setPosition(pos);
	target.draw(shape);
}

void Decor_Sprite::render(sf::RenderTarget& target) const noexcept {
	sprite.setPosition(rec.pos.x, rec.pos.y + rec.size.y);
	if (sprite.getTexture()) {
		sprite.setScale(
			rec.size.x / sprite.getTextureRect().width,
			-rec.size.y / sprite.getTextureRect().height
		);
	}
	sprite.setColor(Vector4d{ 1, 1, 1, opacity * 1. });
	target.draw(sprite);

	if (editor_selected) {
		thread_local std::uint64_t sin_time = 0;
		auto alpha = std::sinf((sin_time += 1) * 0.001f) * 0.5 + 0.25;

		sf::RectangleShape shape;
		shape.setPosition(rec.pos);
		shape.setSize(rec.size);
		shape.setFillColor(Vector4d{ alpha, alpha, alpha, alpha });
		target.draw(shape);
	}
}

void Level::render(sf::RenderTarget& target) const noexcept {
	target.setView(camera);
	auto renders = [&](const auto& cont) { for (const auto& x : cont) x.render(target); };

	renders(doors);
	renders(blocks);
	renders(dry_zones);
	renders(kill_zones);
	renders(next_zones);
	renders(dispensers);
	renders(trigger_zones);

	renders(decor_sprites);

	renders(prest_sources);
	renders(projectiles);
	renders(rocks);

	if (in_editor) camera_bound.render(target, { 1, 0, 0, 1 }, { 0, 0, 0, 0 }, 1);

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
		shape.setPosition(mouse_world_pos);
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
			mouse_world_pos,
			{ 0, 1, 0, 1 }
		);
	}

	for (const auto& x : debug_vectors) {
		Vector2f::renderArrow(target, x.a, x.a + x.b, { 1, 1, 0, 1 }, { 1, 1, 0, 1 });
	}


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
	camera.setSize({ 10, -10 * (window_size.y / (float)window_size.x) });
}

bool Level::test_input(float) noexcept {
	if (in_replay && Level_Store.next_level && IM::isKeyJustPressed(sf::Keyboard::Return)) {
		set_new_level(*Level_Store.next_level);
		return true;
	}

	if (!this_record->focused) return false;

	mouse_screen_pos = this_record->mouse_screen_pos;
	mouse_world_pos = this_record->mouse_world_pos(camera);
	window_size = this_record->window_size;

	if (!in_replay && this_record->is_just_pressed(sf::Keyboard::Quote)) {
		retry();
		return true;
	}

	if (this_record->is_pressed(sf::Keyboard::Q)) {
		if (this_record->is_just_pressed(sf::Keyboard::Q)) player.start_move_sideway();
		player.move_sideway(Player::Right);
	}
	else if (this_record->is_just_released(sf::Keyboard::Q)) {
		player.stop_move_sideway();
	}
	if (this_record->is_pressed(sf::Keyboard::D)) {
		if (this_record->is_just_pressed(sf::Keyboard::D)) player.start_move_sideway();
		player.move_sideway(Player::Left);
	}
	else if (this_record->is_just_released(sf::Keyboard::D)) {
		player.stop_move_sideway();
	}
	if (this_record->is_just_pressed(sf::Keyboard::Space)) {
		if (player.floored || player.coyotee_timer > 0) {
			player.jump();
		}
		else {
			player.preshot_timer = Player::Preshot_Time;
		}
	}
	if (player.jump_strength_modifier_timer > 0) {
		if (this_record->is_pressed(sf::Keyboard::Space)) player.maintain_jump();
		if (this_record->is_pressed(sf::Keyboard::Z)) player.directional_up();
	}
	if (this_record->is_just_pressed(sf::Mouse::Left)) mouse_start_drag();
	if (this_record->is_just_pressed(sf::Mouse::Right)) basic_bindings.clear();
	if (
		this_record->is_pressed(sf::Keyboard::LControl) &&
		this_record->is_just_pressed(sf::Keyboard::Z) &&
		!basic_bindings.empty()
		) {
		basic_bindings.pop_back();
	}
	if (this_record->is_just_pressed(sf::Keyboard::Return)) {
		markers.push_back(player.pos);

		if (this_record->is_pressed(sf::Keyboard::LShift)) markers.clear();
	}

	if (start_drag) {
		if (!this_record->is_pressed(sf::Mouse::Left)) {
			start_drag.reset();
			rock_dragging_i = 0;
		}

		mouse_on_drag();
	}
	
	return false;
}

void Level::update() noexcept {
	for (size_t i = 0; i < decor_sprites.size(); ++i) {
		auto& x = decor_sprites[i];
		if (!x.texture_loaded) {
			// >TODO
			(void)asset::Store.load_texture(x.texture_key, x.texture_path);
			auto texture_size = asset::Store.textures.at(x.texture_key).asset.getSize();

			x.sprite.setTextureRect({ 0, 0, (int)texture_size.x, (int)texture_size.y });
			x.texture_loaded = true;
		}
	}

	this_record = IM::get_iterator();
	if (in_replay && start_record && end_record) {
		if (curr_record == std::next(*end_record)) {
			curr_record = *start_record;
		}
		else {
			this_record = curr_record++;
		}
	}

	auto dt = this_record->dt;
	debug_vectors.clear();

	update_camera(dt);

	if (camera_fade_out_timer > 0) {
		camera_fade_out_timer -= dt;
		if (camera_fade_out_timer <= 0.f) {
			retry();
			input_active_timer = Input_Active_Time;
			camera_fade_in_timer = Camera_Fade_Time;
		}
		return;
	}
	if (camera_fade_out_timer <= 0.f && camera_fade_in_timer > 0) {
		camera_fade_in_timer -= dt;
	}
	
	if (input_active_timer > 0) {
		input_active_timer -= dt;
		if (input_active_timer > 0.f) return;
	}

	if (in_editor || test_input(dt)) return;

	player.update(dt);

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

	auto previous_player = player;
	for (const auto& x : basic_bindings) player.forces += x * Environment.gravity;
	player.forces.y -= Environment.gravity;

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
	bool new_floored = false;

	player.pos.x += velocities.x * dt;
	if (test_any_p(blocks) || test_any_p(doors)) {
		impact += velocities.x * velocities.x;
		player.pos.x = previous_player.pos.x;
		player.velocity.x = 0;
		player.forces.x = 0;
	}
	player.pos.y += velocities.y * dt;
	if (test_any_p(blocks) || test_any_p(doors)) {
		impact += velocities.y * velocities.y;
		player.pos.y = previous_player.pos.y;
		new_floored = velocities.y < 0;
		player.velocity.y = 0;
		player.forces.y = 0;
	}

	if (player.floored && !new_floored) {
		player.coyotee_timer = Player::Coyotee_Time;
	}
	if (!player.floored && new_floored && player.preshot_timer > 0) {
		player.jump();
	}
	player.floored = new_floored;
	
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

		finnish();

		Level_Store.next_level = (Level)*opt_dyn;

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
	Vector2f size = camera.getSize();
	Vector2f target = player.pos;
	if ((target - pos).length2() > camera_idle_radius_2) camera_target = target;

	if (camera_bound.area() > 0) {
		Rectanglef camera_rect = { pos - size / 2, size };
		camera_rect = camera_rect.restrict_in(camera_bound);
		camera.setCenter(camera_rect.center());
	}
}

void Level::retry() noexcept {
	if (Level_Store.initial_level) set_new_level(*Level_Store.initial_level);

	for (auto& x : dispensers) x.set_start_timer();
	speedrun_clock_start = nanoseconds();
}

void Level::finnish() noexcept {
	auto local_start_record = start_record;
	auto local_end_record = in_replay ? end_record : IM::get_iterator();

	retry();

	start_record = local_start_record;
	if (local_start_record) curr_record = *local_start_record;
	end_record = local_end_record;
	in_replay = true;
}

void Level::die() noexcept {
	camera_fade_out_timer = Camera_Fade_Time;
}

void Level::mouse_start_drag() noexcept {
	start_drag = mouse_screen_pos;
	drag_time = seconds();

	for (size_t i = 0; i < rocks.size(); ++i) {
		auto& r = rocks[i];

		Circlef c = { .c = r.pos, .r = r.r + Environment.binding_range };

		if (is_in({ player.pos, player.size }, c) && is_in(mouse_world_pos, c)) {
			auto temp = std::move(rocks[rock_dragging_i]);
			rocks[rock_dragging_i++] = std::move(r);
			r = std::move(temp);
			break;
		}
	}
}

void Level::mouse_on_drag() noexcept {
	auto new_pos = mouse_screen_pos;
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

			// if we started dragging a rock, then we bind it
			for (size_t i = 0; i < rock_dragging_i; ++i) {
				rocks[i].bindings.push_back(unit * prest_gathered);
			}
			if (rock_dragging_i == 0) { // if we started dragging nothing, we default to the player.
				basic_bindings.push_back(unit * prest_gathered);
			}
		}

		start_drag.reset();
		rock_dragging_i = 0;
	}
}

void Level::set_new_level(const Level& l) noexcept {
	*this = l;
	Level_Store.initial_level = l;

	start_record = IM::get_iterator();
	end_record = start_record;
}

void Level::pause() noexcept {}
void Level::resume() noexcept {
	if (start_record == end_record) start_record = IM::get_iterator();

	auto iter = [](auto& x) noexcept { for (auto& y : x) y.editor_selected = false; };

	iter(rocks);
	iter(doors);
	iter(blocks);
	iter(dry_zones);
	iter(next_zones);
	iter(kill_zones);
	iter(dispensers);
	iter(decor_sprites);
	iter(trigger_zones);
	iter(prest_sources);
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
#define X(x) if (has(str, #x)) level.x = (decltype(level.x))str[#x];
	X(blocks);
	X(dispensers);
	X(kill_zones);
	X(next_zones);
	X(prest_sources);
	X(decor_sprites);
	X(dry_zones);
	X(doors);
	X(trigger_zones);
	X(rocks);
	X(markers);
	X(camera_bound)
#undef X

	Player player;
	player.forces = {};
	player.velocity = {};
	player.prest = (float)str["player"]["prest"];
	player.pos = (Vector2f)str["player"]["pos"];
	level.player = player;

	level.camera.setCenter((Vector2f)str["camera"]["pos"]);
	level.camera.setSize((Vector2f)str["camera"]["size"]);
}
void to_dyn_struct(dyn_struct& str, const Level& level) noexcept {
	str = dyn_struct::structure_t{};
	str["blocks"] = level.blocks;
	str["kill_zones"] = level.kill_zones;
	str["next_zones"] = level.next_zones;
	str["dry_zones"] = level.dry_zones;
	str["prest_sources"] = level.prest_sources;
	str["dispensers"] = level.dispensers;
	str["decor_sprites"] = level.decor_sprites;
	str["markers"] = level.markers;
	str["rocks"] = level.rocks;
	str["doors"] = level.doors;
	str["trigger_zones"] = level.trigger_zones;
	str["camera_bound"] = level.camera_bound;

	auto& player = str["player"] = dyn_struct::structure_t{};

	player["prest"] = level.player.prest;
	player["pos"] = level.player.pos;

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
	x.mustnt_triggered = (decltype(x.mustnt_triggered))str["mustnt_triggered"];
}
void to_dyn_struct(dyn_struct& str, const Door& x) noexcept {
	str = dyn_struct::structure_t{};
	str["rec"] = x.rec;
	str["closed"] = x.closed;
	str["must_triggered"] = x.must_triggered;
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
	x.sprite.setTexture(asset::Store.textures.at(x.texture_key).asset);
}
void to_dyn_struct(dyn_struct& str, const Decor_Sprite& x) noexcept {
	str = dyn_struct::structure_t{};
	str["rec"] = x.rec;
	str["texture_path"] = std::filesystem::canonical(x.texture_path).string();
}
