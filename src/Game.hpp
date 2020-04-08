#pragma once
#include <optional>

#include "Level.hpp"
#include "Editor.hpp"
#include "Profil/Profile.hpp"

#include "Managers/InputsManager.hpp"
#include "Graphic/Graphics.hpp"

#include "Screens/Start.hpp"
#include "Screens/Settings.hpp"
#include "Screens/PlayScreen.hpp"
#include "Screens/ProfileSelection.hpp"

struct Game {
	Profile* profile{ nullptr };
	std::vector<Profile> profiles;

	Rectanglef ui_view{ {0, 0}, {1280, 720 } };
	IM::Input_Iterator this_record;

	double timeshots{ 0 };
	bool quit{ false };
	
	void load_start_config() noexcept;

	void new_time(float time) noexcept;

	void input() noexcept;
	void update(std::uint64_t dt) noexcept;
	void render(render::Orders& target) noexcept;
	void render_debug_controller(render::Orders& orders, IM::Input_Iterator it) noexcept;

	Play_Screen play_screen;
	Start_Screen start_screen;
	Settings_Screen settings_screen;
	Profile_Selection_Screen profile_selection_screen;
	
private:
	std::uint64_t fixed_point_timeshot{ 0 };
	std::uint64_t to_carry_over{ 0 };


	void update_step(std::uint64_t dt) noexcept;

	std::optional<Level> load_level(std::filesystem::path path) noexcept;

	Screen* current_screen = &start_screen;
};

extern Game* game;
