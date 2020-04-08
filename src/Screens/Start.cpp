#include "Screens/Start.hpp"

#include "Managers/AssetsManager.hpp"

#include "Game.hpp"
#include "Graphic/UI/Kit.hpp"

void Start_Screen::input(IM::Input_Iterator it) noexcept {
	Vector2f s = { (float)Environment.window_width, (float)Environment.window_height };
	
	kit::get_state().current_origin = { 0.5f, 0.5f };
	kit::get_state().current_pos = { s.x / 2.f, 1 * s.y / 4.f };
	game->quit = kit::button("Quit", 42);
	kit::get_state().current_pos = { s.x / 2.f, 2 * s.y / 4.f };
	goto_settings = kit::button("Config", 42);
	kit::get_state().current_pos = { s.x / 2.f, 3 * s.y / 4.f };
	goto_levels = kit::button("Play", 42);
}

void Start_Screen::update(float dt) noexcept {
}

void Start_Screen::render(render::Orders& target) noexcept {
}

Screen* Start_Screen::next() noexcept {
	if (goto_levels) return &game->profile_selection_screen;
	if (goto_settings) return &game->settings_screen;
	return  nullptr;
}
