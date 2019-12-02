#pragma once

#include <optional>

#include "Graphic/Graphics.hpp"
#include "Managers/InputsManager.hpp"

struct Level_Selection_Screen {

	void input(IM::Input_Iterator it) noexcept;
	void update(float dt) noexcept;
	void render(render::Orders& target) noexcept;
private:


}