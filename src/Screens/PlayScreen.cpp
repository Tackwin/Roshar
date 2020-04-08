#include "PlayScreen.hpp"

#include "Game.hpp"

void Play_Screen::input(IM::Input_Iterator it) noexcept {
	this_record = it;

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
		phantom_paths.clear();
		phantom_paths.push_back({});
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
		auto test_file_save = current_level.file_path.replace_extension(".test");
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

	if (!in_editor) current_level.input(this_record);
	if (!this_record->focused) return;
	if (input_active_timer > 0.f) return;
}

void Play_Screen::update(float dt) noexcept {
	if (to_swap_level) {
		copy_level = *to_swap_level;
		current_level = *to_swap_level;
		to_swap_level = std::nullopt;
		start_record = IM::get_iterator();
		game->timeshots = 0;
		game->profile->current_level = current_level.file_path.generic_string();

		editor.save_path = current_level.file_path.string();

		if (in_full_test) return go_in_test();
		if (in_replay) current_level.feed_phantom_path(phantom_paths);
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
		phantom_paths.back().push_back(current_level.player.get_graphic_state());
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
		phantom_paths.push_back({});
	}
}

void Play_Screen::render(render::Orders& target) noexcept {
	current_level.render(target);

	if (in_editor) {
		current_level.render_debug(target);
		editor.render(target);
	}

	ui_view = {
		{0, 0},
		{(float)Environment.window_width, (float)Environment.window_height}
	};

	target.late_push_view(ui_view);
	defer{ target.late_pop_view(); };


	if (Environment.debug_input) game->render_debug_controller(target, this_record);

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

	char buffer[1024];
	sprintf(buffer, "Level: %s.", current_level.file_path.generic_string().c_str());

	std::string str{buffer};

	target.late_push_text({0, 0}, asset::Font_Id::Consolas, str, 10, {0, 0});
}

Screen* Play_Screen::next() noexcept {
	return nullptr;
}

void Play_Screen::go_in_replay() noexcept {
	end_record = IM::get_iterator();
	begin_record = start_record;
	curr_record = start_record;
	in_replay = true;
	to_swap_level = copy_level;
	in_test = false;
	in_full_test = false;
}

void Play_Screen::go_in_test() noexcept {
	auto test_file = current_level.file_path.replace_extension(".test");
	if (!IM::load_record_at(test_file, RECORD_LEVEL_TEST_ID)) return;

	end_record = --IM::end(RECORD_LEVEL_TEST_ID);
	begin_record = IM::begin(RECORD_LEVEL_TEST_ID);
	curr_record = *begin_record;
	in_test = true;
	in_replay = false;
}

void Play_Screen::go_in_full_test() noexcept {
	in_replay = false;
	in_full_test = true;
}

std::optional<Level> Play_Screen::load_level(std::filesystem::path path) noexcept {
	auto opt_dyn = load_from_json_file(path);
	if (!opt_dyn) {
		printf("Couldn't load file %s.\n", path.string().c_str());
		return std::nullopt;
	}

	Level new_level = (Level)*opt_dyn;
	new_level.file_path = path;
	return new_level;
}

void Play_Screen::new_time(float time) noexcept {
	if (game->profile->best_time.count(current_level.name) == 0)
		game->profile->best_time[current_level.name].best = time;

	game->profile->best_time[current_level.name].last = time;
	auto& it = game->profile->best_time[current_level.name].best;

	it = std::min(time, it);
}
