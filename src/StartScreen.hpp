#pragma once

#include "Managers/InputsManager.hpp"
#include "Graphic/Graphics.hpp"

struct Start_Screen {

	void input(IM::Input_Iterator it) noexcept;
	void update(std::uint64_t dt) noexcept;
	void render(render::Orders& target) noexcept;

	bool goto_settings = false;
	bool goto_levels = false;
	bool exit = false;

private:
	IM::Input_Iterator last_input;

	bool button(render::Orders& target, Vector2f pos,std::string label, float size) noexcept;
};
