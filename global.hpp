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

	std::uint64_t physics_step{ 5'000/*µs*/ }; // 200 ups
	float speed_up_step{ 1 }; // 200 ups

	std::uint32_t window_width{ 0 };
	std::uint32_t window_height{ 0 };
};

extern std::filesystem::path Exe_Path;
extern std::mutex Main_Mutex;
extern Environment_t Environment;

// We need to store this as a global because of the stupid sfml's api decison to permit to query
// the state of the scroll wheel only trough polling...
extern float wheel_scroll;

namespace platform {
	extern void* handle_window;
	extern void* handle_dc_window;
	extern void* main_opengl_context;
	extern void* asset_opengl_context;
}
