#pragma once

#include "Screen.hpp"

#include "Profil/Profile.hpp"

struct Profile_Selection_Screen : public Screen {
	size_t selected = 0;

	virtual void input(IM::Input_Iterator it) noexcept override;
	virtual void update(float dt) noexcept override;
	virtual void render(render::Orders& target) noexcept override;
	virtual Screen* next() noexcept override;
};