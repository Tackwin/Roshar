#pragma once

#include <unordered_map>

#include "Managers/InputsManager.hpp"
#include "Graphic/Graphics.hpp"

struct Start_Screen {

	void input(IM::Input_Iterator it) noexcept;
	void update(float dt) noexcept;
	void render(render::Orders& target) noexcept;

	bool goto_settings = false;
	bool goto_levels = false;
	bool exit = false;

private:
	IM::Input_Iterator last_input = IM::end();

	struct Button_State {
		bool hovering{ false };
		float time_since_hover{ 0.f };
	};

	std::unordered_map<std::string, Button_State> button_states;

	bool button(render::Orders& target, Vector2f pos, std::string label, float size) noexcept;
};
