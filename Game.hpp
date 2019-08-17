#pragma once
#include <optional>
#include <SFML/Graphics.hpp>

#include "Level.hpp"
#include "Editor.hpp"

#include "Managers/InputsManager.hpp"

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


	float camera_speed{ 10 };
	float camera_idle_radius{ 0.3f };
	sf::View camera;


	// >Float >Precision: I'm going to keep increment it by dt step at a dt ~ 1ms i can go up to 
	// 10 000 seconds it should be enough ?
	// keep in mind that it's counting in seconds from the start of the level, so basically a human
	// even trying tricky thing would never spend ~3h on a single level at a 1000 framerate
	// to add up.
	float timeshots{ 0 };

	void input() noexcept;
	void update(float dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;

private:
	float to_carry_over{ 0.f };

	void update_step(float dt) noexcept;

	void update_camera(float dt) noexcept;

	void go_in_test() noexcept;
	void go_in_replay() noexcept;
	void go_in_full_test() noexcept;

	std::optional<Level> load_level(std::filesystem::path path) noexcept;
};

extern Game* game;
