#pragma once

#include <filesystem>

struct Environment_t {
	float gravity{ 9.81f };
	float drag{ 0.8f };
	float gather_speed{ 4 };
	float gather_step{ 0.5f };
	int drag_angle_step{ 16 };

	float dead_velocity{ 10 };

	float window_width;
	float window_height;
};

extern std::filesystem::path Exe_Path;

extern Environment_t Environment;
