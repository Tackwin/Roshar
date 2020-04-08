#include "ProfileSelection.hpp"

#include "Graphic/UI/Kit.hpp"

#include "Game.hpp"

void Profile_Selection_Screen::input(IM::Input_Iterator it) noexcept {

}

void Profile_Selection_Screen::update(float dt) noexcept {
	kit::get_state().current_pos = {
		Environment.window_width  / 4.f,
		Environment.window_height / 4.f
	};
	for (size_t i = 0; i < game->profiles.size(); ++i)
		if (kit::card(game->profiles[i])) selected = i;

	if (auto opt = kit::card_plus()) {
		printf("Selected %s\n", opt->name.c_str());
	}
}

void Profile_Selection_Screen::render(render::Orders& target) noexcept {
}

Screen* Profile_Selection_Screen::next() noexcept {
	return nullptr;
}
