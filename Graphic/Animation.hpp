#pragma once

#include "Math/Rectangle.hpp"
#include "Assets.hpp"

struct dyn_struct;

struct Animation {
	asset::Key sheet_key;

	size_t running_idx{ 0 };
	std::vector<float> rows;

	std::vector<size_t> once_stack;

	Animation(asset::Key sheet) noexcept;

	void update(float dt) noexcept;
	Rectanglef get_rec() const noexcept;
};

struct Animation_Sheet {
	struct Row {
		size_t count{ 0 };
		float time{ 0.f };
	};

	size_t col{ 0 };
	std::vector<Row> rows;
};

extern void from_dyn_struct(const dyn_struct& str, Animation_Sheet& animation) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Animation_Sheet& animation) noexcept;

