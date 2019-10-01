#pragma once

#include "Math/Rectangle.hpp"

struct dyn_struct;

struct Animation {
	struct Row {
		size_t count{ 0 };
		float time{ 0.f };
		float t{ 0.f };
	};

	size_t col{ 0 };
	std::vector<Row> rows;

	void update(size_t idx, float dt) noexcept;
	Rectanglef get_rec(size_t idx) noexcept;
};

extern void from_dyn_struct(const dyn_struct& str, Animation& animation) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Animation& animation) noexcept;

