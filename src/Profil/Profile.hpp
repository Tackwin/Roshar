#pragma once

#include <unordered_map>
#include "Managers/InputsManager.hpp"
#include "dyn_struct.hpp"

struct Control_Bindings {
	struct Action {
		std::optional<Keyboard::Key> key;
		std::optional<Mouse::Button> button;
		std::optional<Joystick::Button> controller;
	};

	Action jump = { Keyboard::Key::Space, Mouse::Button::Count, Joystick::Button::RB };
	Action up = { Keyboard::Key::Z };
	Action left = { Keyboard::Key::Q };
	Action down = { Keyboard::Key::S };
	Action right = { Keyboard::Key::D };
	Action grap = { Keyboard::Key::LCTRL, std::nullopt, Joystick::Button::LB };
	Action clear = { std::nullopt, Mouse::Button::Right, Joystick::Button::B };
	Action cancel = { Keyboard::Key::Z, std::nullopt, Joystick::Button::B };

	Keyboard::Key slow = Keyboard::Key::LSHIFT;

	Mouse::Button start_drag;
	Mouse::Button clear_bindings;
};

extern void to_dyn_struct(dyn_struct& str, const Control_Bindings& bindings) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Control_Bindings& bindings) noexcept;

struct Profile {
	struct Time_Score {
		float best;
		float last;
	};
	std::string name;
	
	std::unordered_map<std::string, Time_Score> best_time;
	Control_Bindings bindings;

	std::string current_level;

	float uptime = .0f;
};

extern void to_dyn_struct(dyn_struct& str, const Profile& profile) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Profile& profile) noexcept;
