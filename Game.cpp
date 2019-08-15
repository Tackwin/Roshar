#include "Game.hpp"

#include "dyn_struct.hpp"

Game* game = nullptr;

void Game::input() noexcept {
	if (!IM::isWindowFocused()) return;

	this_record = IM::get_iterator();
	if ((in_replay || in_test) && begin_record && end_record) {
		if (curr_record == std::next(*end_record)) {
			curr_record = *begin_record;
			this_record = curr_record;
		}
		else {
			this_record = curr_record++;
		}
	}

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

	if (!this_record->focused) return;

	if (!in_replay && !in_test && this_record->is_just_pressed(sf::Keyboard::Quote)) {
		to_swap_level = copy_level;
	}

	if (!in_editor) current_level.input(this_record);
}

void Game::update() noexcept {
	auto dt = this_record->dt;
	if (to_swap_level) {
		copy_level = *to_swap_level;
		current_level = *to_swap_level;
		to_swap_level = std::nullopt;
		start_record = IM::get_iterator();

		if (in_full_test) return go_in_test();
	}

	if (!in_editor) {
		if (camera_fade_out_timer > 0.f) {
			camera_fade_out_timer -= dt;
			if (camera_fade_out_timer > 0.f) return;

			to_swap_level = copy_level;
			input_active_timer = Input_Active_Time;
			camera_fade_in_timer = Camera_Fade_Time;
		}

		
		input_active_timer -= dt;
		camera_fade_in_timer -= dt;

		if (input_active_timer > 0.f) return;

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

void Game::render(sf::RenderTarget& target) noexcept {
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
	if (!IM::load_record_at(test_file, RECORD_LEVEL_TEST_ID)) {
		printf("Failed to load\n");
		return;
	}

	end_record = --IM::end(RECORD_LEVEL_TEST_ID);
	begin_record = IM::begin(RECORD_LEVEL_TEST_ID);
	curr_record = *begin_record;
	in_test = true;
	in_replay = false;
}

void Game::go_in_full_test() noexcept {
	go_in_test();
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
