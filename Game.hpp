#pragma once
#include <optional>

#include "Level.hpp"
#include "Editor.hpp"

#include "Managers/InputsManager.hpp"
#include "Graphic/Graphics.hpp"

struct Game {
	Editor editor;

	Level copy_level;
	Level current_level;
	std::optional<Level> to_swap_level;

	bool in_replay{ false };
	bool in_test{ false };
	bool in_full_test{ false };
	bool in_editor{ false };

	IM::Input_Iterator this_record;
	IM::Input_Iterator curr_record;
	IM::Input_Iterator start_record;

	std::optional<IM::Input_Iterator> begin_record;
	std::optional<IM::Input_Iterator> end_record;

	bool died{ false };
	bool succeed{ false };
	std::filesystem::path next_level_path;

	static constexpr float Input_Active_Time = 0.5f;
	float input_active_timer = Input_Active_Time;

	static constexpr float Camera_Fade_Time = 0.5f;
	float camera_fade_out_timer = 0;
	float camera_fade_in_timer = 0;


	Rectanglef ui_view{ {0, 0}, {1280, 720 } };

	double timeshots{ 0 };
	
	void input() noexcept;
	void update(std::uint64_t dt) noexcept;
	void render(render::Orders& target) noexcept;

private:
	std::uint64_t fixed_point_timeshot{ 0 };
	std::uint64_t to_carry_over{ 0 };

	void render_debug_controller(render::Orders& orders, IM::Input_Iterator it) noexcept;

	void update_step(std::uint64_t dt) noexcept;

	void go_in_test() noexcept;
	void go_in_replay() noexcept;
	void go_in_full_test() noexcept;

	std::optional<Level> load_level(std::filesystem::path path) noexcept;
};

extern Game* game;
