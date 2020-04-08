#pragma once

#include <unordered_map>

#include "Managers/InputsManager.hpp"
#include "Graphic/Graphics.hpp"

#include "Screen.hpp"

#include "Profil/Profile.hpp"

struct Settings_Screen : public Screen {

	virtual void input(IM::Input_Iterator it) noexcept override;
	virtual void update(float dt) noexcept override;
	virtual void render(render::Orders& target) noexcept override;
	virtual Screen* next() noexcept override;

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

	Keyboard::Key key_prompt(
		render::Orders& target,
		Vector2f pos,
		std::string label,
		float size,
		Keyboard::Key* binded
	) noexcept;
};
