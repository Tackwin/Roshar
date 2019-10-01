#include "Animation.hpp"

#include "Managers/AssetsManager.hpp"

#include <cassert>

Animation::Animation(asset::Key sheet) noexcept : sheet_key(sheet) {
	Animation_Sheet& s = asset::Store.get_animation(sheet_key);
	rows.resize(s.rows.size());
}

void Animation::update(float dt) noexcept {
	Animation_Sheet& sheet = asset::Store.get_animation(sheet_key);

	if (once_stack.empty()) {
		rows[running_idx] += dt;
		if (rows[running_idx] >= sheet.rows[running_idx].time)
			rows[running_idx] = 0;
	}
	else {
		size_t idx = once_stack.back();
		rows[idx] += dt;
		if (rows[idx] >= sheet.rows[idx].time) once_stack.pop_back();
	}
}

Rectanglef Animation::get_rec() const noexcept {
	Animation_Sheet& sheet = asset::Store.get_animation(sheet_key);

	size_t idx = once_stack.empty() ? running_idx : once_stack.back();

	auto& current = rows[idx];
	auto index = (size_t)((current / sheet.rows[idx].time) * sheet.rows[idx].count);

	return {
		(float)index / sheet.col,
		(float)idx   / sheet.rows.size(),
		1.f          / sheet.col,
		1.f          / sheet.rows.size()
	};
}

void from_dyn_struct(const dyn_struct& str, Animation_Sheet& animation) noexcept {
	animation.rows.reserve(size(str));
	for (const auto& x : iterate_array(str)) {
		Animation_Sheet::Row row;
		row.count = (size_t)x["count"];
		row.time = (float)x["time"];

		animation.col = std::max(row.count, animation.col);
		animation.rows.push_back(row);
	}
}
void to_dyn_struct(dyn_struct&, const Animation_Sheet&) noexcept {
	// >TODO(Tackwin);

	assert(false);
}


