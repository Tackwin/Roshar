#include "Screens/Settings.hpp"

#include "Managers/AssetsManager.hpp"

#include "Game.hpp"
#include "Profil/Profile.hpp"
#include "Graphic/UI/Kit.hpp"

void Settings_Screen::input(IM::Input_Iterator it) noexcept {
	last_input = it;
	auto& bindings = game->profile->bindings;
	
	kit::get_state().current_pos = {50, 50};
	kit::get_state().current_origin = {0, 0};
	go_back = kit::button("Back", 42);

	kit::get_state().current_pos = {
		Environment.window_width / 3.f, Environment.window_height * 0.25f
	};
	kit::key_prompt("Jump  : ", 21, bindings.jump);
	kit::key_prompt("Up    : ", 21, bindings.up);
	kit::key_prompt("Left  : ", 21, bindings.left);
	kit::key_prompt("Down  : ", 21, bindings.down);
	kit::key_prompt("Right : ", 21, bindings.right);
	kit::key_prompt("Slow  : ", 21, bindings.slow);
	kit::key_prompt("Grap  : ", 21, bindings.grap);
	kit::key_prompt("Cancel: ", 21, bindings.cancel);
	kit::key_prompt("Clear : ", 21, bindings.clear);
}

void Settings_Screen::update(float dt) noexcept {
}

void Settings_Screen::render(render::Orders& target) noexcept {
}

Screen* Settings_Screen::next() noexcept {
	if (go_back) return &game->start_screen;
	return nullptr;
}
