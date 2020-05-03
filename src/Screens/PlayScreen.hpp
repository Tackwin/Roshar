#pragma once

#include "Screen.hpp"
#include "Level.hpp"
#include "Editor.hpp"
#include "Profil/Profile.hpp"

#include "Managers/InputsManager.hpp"
#include "Graphic/Graphics.hpp"

class Play_Screen : public Screen {
public:
	Editor editor;

	std::optional<Level> to_swap_level;
	Level current_level;
	Level copy_level;

	bool in_replay{ false };
	bool in_test{ false };
	bool in_full_test{ false };
	bool in_editor{ false };
	bool in_menu{ false };

	bool go_main_menu{false};

	Rectanglef ui_view{ {0, 0}, {1280, 720 } };

	IM::Input_Iterator this_record;
	IM::Input_Iterator curr_record;
	IM::Input_Iterator start_record;

	std::optional<IM::Input_Iterator> begin_record;
	std::optional<IM::Input_Iterator> end_record;

	std::vector<std::vector<Player::Graphic_State>> phantom_paths{ {} };

	bool died{ false };
	bool succeed{ false };
	std::filesystem::path next_level_path;

	static constexpr float Input_Active_Time = 0.5f;
	float input_active_timer = Input_Active_Time;

	static constexpr float Camera_Fade_Time = 0.5f;
	float camera_fade_out_timer = 0;
	float camera_fade_in_timer = 0;

	double start_level_time = 0;

	void go_in_test() noexcept;
	void go_in_replay() noexcept;
	void go_in_full_test() noexcept;
	void go_to_gameplay() noexcept;

public:
	std::optional<Level> load_level(std::filesystem::path path) noexcept;

	void input(IM::Input_Iterator it) noexcept;
	void input_menu(IM::Input_Iterator it) noexcept;
	void update(float dt) noexcept;
	void update_menu(float dt) noexcept;
	void render(render::Orders& target) noexcept;
	void render_menu(render::Orders& target) noexcept;

	Screen* next() noexcept;
	void new_time(float time) noexcept;
};