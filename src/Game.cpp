#include "Game.hpp"

#include "dyn_struct.hpp"
#include "Graphic/Graphics.hpp"

#include "Graphic/UI/Kit.hpp" 

Game* game = nullptr;

void Game::load_start_config() noexcept {
	auto start_opt = load_from_json_file("assets/config.json");
	if (!start_opt) return;
	auto start = *start_opt;

	if (has(start, "start_level")) {
		play_screen.to_swap_level = load_level((std::string)start["start_level"]);
	}
	if (has(start, "current_screen")) {
		#define X(x, y) if ((std::string)start["current_screen"] == #x) current_screen = y;
		X(Play, &play_screen);
		X(Start, &start_screen);
		X(Settings, &settings_screen);
		X(ProfileSelect, &profile_selection_screen);
		X(None, nullptr);
		#undef X
	}
	if (has(start, "controller_idx")) {
		IM::controller_idx = (size_t)start["controller_idx"];
	}
	if (has(start, "profiles")) {
		profiles = (std::vector<Profile>)start["profiles"];
	}
}

void Game::input() noexcept {
	IM::update(Environment.physics_step / 1'000'000.f);

	this_record = IM::get_iterator();
	if (!IM::isWindowFocused()) return;

	kit::input(this_record);
	if (current_screen) {
		current_screen->input(this_record);
		return;
	}

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
	kit::update(dt);

	if (current_screen) {
		current_screen->update(dt);
		if (auto ptr = current_screen->next()) current_screen = ptr;
		return;
	}

	if (quit) application_running = false;
}

void Game::render(render::Orders& target) noexcept {
	if (current_screen) {
		current_screen->render(target);
		return;
	}
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
std::optional<Level> Game::load_level(std::filesystem::path path) noexcept {
	auto opt_dyn = load_from_json_file(path);
	if (!opt_dyn) {
		printf("Couldn't load file %s.\n", path.string().c_str());
		return std::nullopt;
	}

	Level new_level = (Level)*opt_dyn;
	new_level.file_path = path;
	return new_level;
}
