#include "Animation.hpp"

#include <cassert>

void Animation::update(size_t idx, float dt) noexcept {
	auto& current = rows[idx];

	current.t += dt;
	if (current.t > current.time) current.t = 0.f;
}

Rectanglef Animation::get_rec(size_t idx) noexcept {
	auto& current = rows[idx];
	auto index = (size_t)((current.t / current.time) * current.count);

	return {
		(float)index / col,
		(float)idx   / rows.size(),
		1.f          / col,
		1.f          / rows.size()
	};
}

void from_dyn_struct(const dyn_struct& str, Animation& animation) noexcept {
	animation.rows.reserve(size(str));
	for (const auto& x : iterate_array(str)) {
		Animation::Row row;
		row.count = (size_t)x["count"];
		row.time = (float)x["time"];
		row.t = 0.f;

		animation.col = std::max(row.count, animation.col);
		animation.rows.push_back(row);
	}
}
void to_dyn_struct(dyn_struct&, const Animation&) noexcept {
	// >TODO(Tackwin);

	assert(false);
}


