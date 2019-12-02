#pragma once

#include <unordered_map>

#include "Managers/InputsManager.hpp"
#include "Graphic/Graphics.hpp"

struct Control_Bindings {
	struct Action {
		std::optional<Keyboard::Key> key;
		std::optional<Mouse::Button> button;
		std::optional<Joystick::Button> controller;

		std::optional<Vector2f> left_joystick_vector;
	};

	Action jump = { Keyboard::Key::Space, Mouse::Button::Count, Joystick::Button::RB };
	Action up = { Keyboard::Key::Z };
	Action left = { Keyboard::Key::Q };
	Action down = { Keyboard::Key::S };
	Action right = { Keyboard::Key::D };
	
	Keyboard::Key slow;

	Mouse::Button start_drag;
	Mouse::Button clear_bindings;
};

struct Settings_Screen {

	void input(IM::Input_Iterator it) noexcept;
	void update(float dt) noexcept;
	void render(render::Orders& target) noexcept;

	bool go_back{ false };

private:

	IM::Input_Iterator last_input = IM::end();

	struct Button_State {
		bool hovering{false};
		float time_since_hover{0};
	};
	struct Key_Prompt {
		bool hovering{false};
		bool focused{false};
	};

	std::unordered_map<std::string, Button_State> button_states;
	std::unordered_map<std::string, Key_Prompt> key_prompts;

	bool button(
		render::Orders& target, Vector2f pos, std::string label, float size, Vector2f origin
	) noexcept;

	Keyboard::Key key_prompt(
		render::Orders& target,
		Vector2f pos,
		std::string label,
		float size,
		Control_Bindings::Action* binded
	) noexcept;
};
