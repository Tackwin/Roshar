#pragma once

#include <unordered_map>

#include "Managers/InputsManager.hpp"
#include "Graphic/Graphics.hpp"

#include "Screen.hpp"

struct Start_Screen : public Screen {

	virtual void input(IM::Input_Iterator it) noexcept override;
	virtual void update(float dt) noexcept override;
	virtual void render(render::Orders& target) noexcept override;
	virtual Screen* next() noexcept override;

	bool goto_settings = false;
	bool goto_levels = false;
};
