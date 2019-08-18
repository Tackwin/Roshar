#include "Game.hpp"

#include "dyn_struct.hpp"

Game* game = nullptr;

void Game::input() noexcept {
	IM::update(Environment.physics_step / 1'000'000.f);

	this_record = IM::get_iterator();
	if (!IM::isWindowFocused()) return;
	
	if (IM::isKeyJustPressed(sf::Keyboard::E)) {
		in_editor = !in_editor;
		if (!in_editor) {
			current_level.resume();
		}
		else {
			current_level.pause();
		}
	}

	if (in_replay && IM::isKeyJustPressed(sf::Keyboard::Return)) {
		to_swap_level = load_level(next_level_path);
		in_replay = false;
	}
	if (IM::isKeyPressed(sf::Keyboard::Escape)) {
		if (in_replay || in_full_test || in_test) {
			to_swap_level = copy_level;
			in_test = false;
			in_replay = false;
			in_full_test = false;
		}
	}

	if (IM::isKeyJustPressed(sf::Keyboard::F10)) {
		to_swap_level = copy_level;
		return go_in_full_test();
	}
	if (in_test && IM::isKeyJustPressed(sf::Keyboard::Return)) {
		in_test = false;
	}

	if (in_replay && IM::isKeyJustPressed(sf::Keyboard::F11)) {
		auto test_file_save = current_level.save_path.replace_extension(".test");
		IM::save_range(test_file_save, *begin_record, std::next(*end_record));
	}

	if (!in_replay && IM::isKeyJustPressed(sf::Keyboard::F11)) {
		to_swap_level = copy_level;
		return go_in_test();
	}
	if (IM::isKeyJustPressed(sf::Keyboard::Quote)) {
		died = true;
	}

	if (camera_fade_out_timer > 0.f) {
		return;
	}

	if ((in_replay || in_test) && begin_record && end_record) {
		if (curr_record == std::next(*end_record)) {
			curr_record = *begin_record;
			this_record = curr_record;
		}
		else {
			this_record = curr_record++;
		}
	}

	if (!this_record->focused) return;
	if (input_active_timer > 0.f) return;
}

void Game::update(std::uint64_t dt) noexcept {
	dt = (std::uint64_t)(dt * (double)Environment.speed_up_step);

	dt += to_carry_over;
	std::uint64_t to_do = dt / Environment.physics_step;
	to_carry_over = dt - to_do * Environment.physics_step;

	for (size_t i = 0; i < to_do; ++i) update_step(Environment.physics_step);
}

void Game::update_step(std::uint64_t fixed_dt) noexcept {
	fixed_point_timeshot += fixed_dt;
	auto dt = fixed_dt / 1'000'000.f;

	timeshots = fixed_point_timeshot / 1'000'000.0;

	input();

	if (!in_editor) current_level.input(this_record);

	if (to_swap_level) {
		copy_level = *to_swap_level;
		current_level = *to_swap_level;
		to_swap_level = std::nullopt;
		start_record = IM::get_iterator();
		timeshots = 0;

		camera.setSize(
			current_level.camera_start.size.x,
			-current_level.camera_start.size.y
		);
		camera.setCenter(current_level.camera_start.center());
		editor.save_path = current_level.save_path.string();

		if (in_full_test) return go_in_test();
	}

	if (!in_editor) {
		update_camera(dt);

		if (camera_fade_out_timer > 0.f) {
			camera_fade_out_timer -= dt;
			if (camera_fade_out_timer > 0.f) return;

			to_swap_level = copy_level;
			input_active_timer = Input_Active_Time;
			camera_fade_in_timer = Camera_Fade_Time;
			return;
		}


		input_active_timer -= dt;
		camera_fade_in_timer -= dt;

		current_level.update(dt);
	}
	else {
		editor.update(dt);
	}

	if (succeed) {
		if (!in_test && !in_replay) {
			// >TODO(Tackwin): go in replay mode. Don't forget to setup your input iterators.
			go_in_replay();
		}
		else if (in_test) {
			to_swap_level = load_level(next_level_path);

			if (!in_full_test) {
				in_test = false;
				begin_record.reset();
				end_record.reset();
			}
		}
		else if (in_replay) {
			to_swap_level = copy_level;

			curr_record = *begin_record;
		}

		succeed = false;
	}

	if (died) {
		camera_fade_out_timer = Camera_Fade_Time;
		died = false;
	}

}

void Game::update_camera(float dt) noexcept {
	auto camera_target = current_level.player.pos;
	auto camera_pos = (Vector2f)camera.getCenter();
	auto camera_size = (Vector2f)camera.getSize();
	auto dist = (camera_target - camera_pos).length();

	if (dist > camera_idle_radius) {
		Vector2f dt_pos = camera_target - camera_pos;
		Vector2f to_move =
			std::min(dist - camera_idle_radius, dt * camera_speed) * dt_pos.normalize();
		camera.move(to_move);
		camera_pos += to_move;
	}

	if (current_level.camera_bound.area() > 0) {
		Rectanglef camera_rect = { camera_pos - camera_size / 2, camera_size };
		camera_rect = camera_rect.restrict_in(current_level.camera_bound);
		camera.setCenter(camera_rect.center());
	}
}

void Game::render(sf::RenderTarget& target) noexcept {
	target.setView(camera);

	current_level.render(target);
	if (in_editor) editor.render(target);

	auto view = target.getView();
	defer{ target.setView(view); };
	target.setView(target.getDefaultView());
	sf::RectangleShape shape(
		{ (float)Environment.window_width, (float)Environment.window_height }
	);
	shape.setPosition(0, 0);
	if (camera_fade_out_timer > 0) {
		auto alpha = 1 - (camera_fade_out_timer / Camera_Fade_Time);

		shape.setFillColor(Vector4d{ 0, 0, 0, alpha });
		target.draw(shape);
	}
	if (camera_fade_out_timer <= 0 && camera_fade_in_timer > 0) {
		auto alpha = camera_fade_in_timer / Camera_Fade_Time;
		auto gray = 1 - alpha;

		shape.setFillColor(Vector4d{ gray, gray, gray, alpha });
		target.draw(shape);
	}
}

void Game::go_in_replay() noexcept {
	end_record = IM::get_iterator();
	begin_record = start_record;
	curr_record = start_record;
	in_replay = true;
	to_swap_level = copy_level;
	in_test = false;
	in_full_test = false;
}

void Game::go_in_test() noexcept {
	auto test_file = current_level.save_path.replace_extension(".test");
	if (!IM::load_record_at(test_file, RECORD_LEVEL_TEST_ID)) return;

	end_record = --IM::end(RECORD_LEVEL_TEST_ID);
	begin_record = IM::begin(RECORD_LEVEL_TEST_ID);
	curr_record = *begin_record;
	in_test = true;
	in_replay = false;
}

void Game::go_in_full_test() noexcept {
	in_replay = false;
	in_full_test = true;
}

std::optional<Level> Game::load_level(std::filesystem::path path) noexcept {
	auto opt_dyn = load_from_json_file(path);
	if (!opt_dyn) {
		printf("Couldn't load file %s.\n", path.string().c_str());
		return std::nullopt;
	}

	Level new_level = (Level)* opt_dyn;
	new_level.save_path = path;
	return new_level;
}
