#pragma once

#include "Managers/InputsManager.hpp"

class Screen {
public:
	virtual ~Screen() noexcept = default;

	virtual void input(IM::Input_Iterator it) noexcept = 0;
	virtual void update(float dt) noexcept = 0;
	virtual void render(render::Orders& target) noexcept = 0;
	virtual Screen* next() noexcept = 0;
};