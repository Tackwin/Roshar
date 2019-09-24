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

void Game::load_start_config() noexcept {
	auto start_opt = load_from_json_file("start_config.json");
	if (!start_opt) return;
	auto start = *start_opt;

	if (has(start, "start_level")) {
		to_swap_level = load_level((std::string)start["start_level"]);
	}
}

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

		editor.save_path = current_level.save_path.string();

		if (in_full_test) return go_in_test();
	}

	if (!in_editor) {
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

void Game::render(render::Orders& target) noexcept {
	current_level.render(target);
	if (in_editor) editor.render(target);

	ui_view = {
		{0, 0},
		{(float)Environment.window_width, (float)Environment.window_height}
	};

	target.late_push_view(ui_view);
	defer{ target.late_pop_view(); };


	if (Environment.debug_input) render_debug_controller(target, this_record);

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

void Game::render_debug_controller(render::Orders& orders, IM::Input_Iterator it) noexcept {
	Vector4d white = { 1, 1, 1, 1 };
	Vector4d black = { 0, 0, 0, 1 };
	Vector4d blue = { 0, 0, 1, 1 };
	Vector4d red = { 1, 0, 0, 1 };
	Vector4d green = { 0, 1, 0, 1 };
	Vector4d yellow = { 1, 1, 0, 1 };
	Vector4d pink = { 1, 0, 1, 1 };

	orders.late_push_rectangle({ 100, 200 }, { 100, 100 }, { 1, 1, 1, 1 });
	orders.late_push_rectangle({ 102.5f, 202.5f }, { 95, 95 }, { 0, 0, 0, 1 });
	orders.late_push_circle(
		5,
		Vector2f{ 150, 250 } + it->left_joystick * 50,
		it->is_pressed(Joystick::LT) ? white : pink
	);
	orders.late_push_rectangle({ 300, 100 }, { 100, 100 }, { 1, 1, 1, 1 });
	orders.late_push_rectangle({ 302.5f, 102.5f }, { 95, 95 }, { 0, 0, 0, 1 });
	orders.late_push_circle(
		5,
		Vector2f{ 350, 150 } + it->right_joystick * 50,
		it->is_pressed(Joystick::RT) ? white : pink
	);

	orders.late_push_circle(5, { 240, 250 }, it->is_pressed(Joystick::BACK) ? white : black);
	orders.late_push_circle(5, { 260, 250 }, it->is_pressed(Joystick::START) ? white : black);

	orders.late_push_circle(10, { 350, 275 }, it->is_pressed(Joystick::Y) ? yellow : black);
	orders.late_push_circle(10, { 325, 250 }, it->is_pressed(Joystick::X) ? blue : black);
	orders.late_push_circle(10, { 350, 225 }, it->is_pressed(Joystick::A) ? green : black);
	orders.late_push_circle(10, { 375, 250 }, it->is_pressed(Joystick::B) ? red : black);

	orders.late_push_rectangle(
		{ 145, 160 }, { 10, 30 }, it->is_pressed(Joystick::DPAD_UP) ? white : black
	);
	orders.late_push_rectangle(
		{ 110, 145 }, { 30, 10 }, it->is_pressed(Joystick::DPAD_LEFT) ? white : black
	);
	orders.late_push_rectangle(
		{ 145, 110 }, { 10, 30 }, it->is_pressed(Joystick::DPAD_DOWN) ? white : black
	);
	orders.late_push_rectangle(
		{ 160, 145 }, { 30, 10 }, it->is_pressed(Joystick::DPAD_RIGHT) ? white : black
	);

	orders.late_push_rectangle(
		{ 120, 310 }, { 60, 20 }, it->is_pressed(Joystick::LB) ? white : black
	);
	orders.late_push_rectangle(
		{ 320, 310 }, { 60, 20 }, it->is_pressed(Joystick::RB) ? white : black
	);

	auto amount = (double)this_record->left_trigger;
	Vector4d color = { amount, amount, amount, 1 };
	orders.late_push_circle(10, { 150, 350 }, color);

	amount = (double)this_record->right_trigger;
	color = { amount, amount, amount, 1 };
	orders.late_push_circle(10, { 350, 350 }, color);
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
