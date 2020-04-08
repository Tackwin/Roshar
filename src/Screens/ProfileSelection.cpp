#include "ProfileSelection.hpp"

#include "Graphic/UI/Kit.hpp"

#include "Game.hpp"

void Profile_Selection_Screen::input(IM::Input_Iterator it) noexcept {
	kit::get_state().current_pos = {
		Environment.window_width  / 4.f,
		Environment.window_height / 4.f
	};

	if (auto opt = kit::card_plus()) {
		add_new_profile(opt->name);
		game->profile = &game->profiles.back();
		enter_game = true;
	}
	for (size_t i = 0; i < game->profiles.size(); ++i) {
		if (kit::card(game->profiles[i])) {
			selected = i;
			game->profile = &game->profiles[i];
			game->play_screen.to_swap_level =
				game->play_screen.load_level(game->profile->current_level);
		}
	}
}

void Profile_Selection_Screen::update(float dt) noexcept {
}

void Profile_Selection_Screen::render(render::Orders& target) noexcept {
}

Screen* Profile_Selection_Screen::next() noexcept {
	return (enter_game || selected.has_value()) ? &(game->play_screen) : nullptr;
}

void Profile_Selection_Screen::add_new_profile(std::string name) noexcept {
	Profile p{};
	p.current_level = Level::Start_Level;
	p.name = name;
	game->profiles.push_back(p);
}

