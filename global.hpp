#pragma once

struct Environment_t {
	float gravity{ 981 };
	float drag{ 0.4f };
	int drag_angle_step = 16;
};

extern Environment_t Environment;
