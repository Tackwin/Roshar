#pragma once

#include "Screen.hpp"

class Level_Screen : public Screen {

public:
	virtual void input(IM::Input_Iterator it) noexcept override;
	virtual void update(float dt) noexcept override;
	virtual void render(render::Orders& target) noexcept override;

};