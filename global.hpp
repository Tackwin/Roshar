#pragma once

struct Environment_t {
	float gravity{ 9.81f };
	float drag{ 0.4f };
	float gather_speed{ 4 };
	float gather_step{ 0.5f };
	int drag_angle_step{ 16 };
};

extern Environment_t Environment;
