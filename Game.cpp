#include "Game.hpp"

#include "dyn_struct.hpp"
#include "Graphic/Graphics.hpp"

void render_game(render::Orders& o) noexcept {
	game->render(o);
}
void update_game(std::uint64_t dt) noexcept {
	game->update(dt);
}

Game* game = nullptr;

void Game::input() noexcept {
	IM::update(Environment.physics_step / 1'000'000.f);

	this_record = IM::get_iterator();
	if (!IM::isWindowFocused()) return;
	
	if (IM::isKeyJustPressed(Keyboard::E)) {
		in_editor = !in_editor;
		if (!in_editor) {
			current_level.resume();
		}
		else {
			current_level.pause();
		}
	}

	if (
		in_replay &&
		(IM::isKeyJustPressed(Keyboard::Return) || IM::isKeyJustPressed(Joystick::A))
	) {
		to_swap_level = load_level(next_level_path);
		in_replay = false;
	}
	if (IM::isKeyPressed(Keyboard::Escape)) {
		if (in_replay || in_full_test || in_test) {
			to_swap_level = copy_level;
			in_test = false;
			in_replay = false;
			in_full_test = false;
		}
	}

	if (IM::isKeyJustPressed(Keyboard::F10)) {
		to_swap_level = copy_level;
		return go_in_full_test();
	}
	if (in_test && IM::isKeyJustPressed(Keyboard::Return)) {
		in_test = false;
	}

	if (in_replay && IM::isKeyJustPressed(Keyboard::F11)) {
		auto test_file_save = current_level.save_path.replace_extension(".test");
		IM::save_range(test_file_save, *begin_record, std::next(*end_record));
	}

	if (!in_replay && IM::isKeyJustPressed(Keyboard::F11)) {
		to_swap_level = copy_level;
		return go_in_test();
	}
	if (IM::isKeyJustPressed(Keyboard::Quote) || IM::isKeyJustPressed(Joystick::Y)) {
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

		camera = current_level.camera_start;
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
	auto camera_center = camera.center();
	auto dist = (camera_target - camera_center).length();

	if (dist > camera_idle_radius) {
		Vector2f dt_pos = camera_target - camera_center;
		Vector2f to_move =
			std::min(dist - camera_idle_radius, dt * camera_speed) * dt_pos.normalize();
		camera.pos += to_move;
	}

	if (current_level.camera_bound.area() > 0)
		camera = camera.restrict_in(current_level.camera_bound);

}

void Game::render(render::Orders& target) noexcept {
	target.push_view(camera);

	current_level.render(target);
	if (in_editor) editor.render(target);

	ui_view = {
		{0, 0},
		{(float)Environment.window_width, (float)Environment.window_height}
	};

	target.late_push_view(ui_view);
	defer{ target.late_pop_view(); };

	Vector4d color = { 0, 0, 0, 0 };
	if (camera_fade_out_timer > 0) {
		auto alpha = 1 - (camera_fade_out_timer / Camera_Fade_Time);

		color = { 0, 0, 0, alpha };
	}
	if (camera_fade_out_timer <= 0 && camera_fade_in_timer > 0) {
		auto alpha = camera_fade_in_timer / Camera_Fade_Time;
		auto gray = 1 - alpha;

		color = { gray, gray, gray, alpha };
	}

	target.late_push_rectangle(
		{ 0, 0 },
		{ (float)Environment.window_width, (float)Environment.window_height },
		color
	);
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
