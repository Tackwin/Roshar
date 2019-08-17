#pragma once

#include <filesystem>
#include <mutex>

struct Environment_t {
	float gravity{ 9.81f };
	float drag{ 0.8f };
	float gather_speed{ 4 };
	float gather_step{ 0.5f };
	int drag_angle_step{ 16 };

	float dead_velocity{ 10 };

	float binding_range{ 1 };

	float physics_step{ 0.005f }; // 200 ups

	std::uint32_t window_width;
	std::uint32_t window_height;
};

extern std::filesystem::path Exe_Path;
extern std::mutex Main_Mutex;
extern Environment_t Environment;

namespace sf {
	class RenderWindow;
};
extern sf::RenderWindow* window;

// We need to store this as a global because of the stupid sfml's api decison to permit to query
// the state of the scroll wheel only trough polling...
extern float wheel_scroll;
