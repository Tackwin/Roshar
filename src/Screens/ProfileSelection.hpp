#pragma once

#include <optional>
#include "Screen.hpp"

#include "Profil/Profile.hpp"

struct Profile_Selection_Screen : public Screen {
	std::optional<size_t> selected = std::nullopt;

	virtual void input(IM::Input_Iterator it) noexcept override;
	virtual void update(float dt) noexcept override;
	virtual void render(render::Orders& target) noexcept override;
	virtual Screen* next() noexcept override;

private:
	bool enter_game = false;

	void add_new_profile(std::string name) noexcept;
};