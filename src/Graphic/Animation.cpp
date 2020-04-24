#include "Animation.hpp"

#include "Managers/AssetsManager.hpp"

#include <cassert>

Animation::Animation(asset::Key sheet) noexcept : sheet_key(sheet) {
	Animation_Sheet& s = asset::Store.get_animation(sheet_key);
	rows.resize(s.entries.size());
}

void Animation::update(float dt) noexcept {
	Animation_Sheet& sheet = asset::Store.get_animation(sheet_key);

	if (once_stack.empty()) {
		rows[running_idx] += dt;
		if (rows[running_idx] >= sheet.entries[running_idx].time)
			rows[running_idx] = 0;
	}
	else {
		size_t idx = once_stack.back();
		rows[idx] += dt;
		if (rows[idx] >= sheet.entries[idx].time) {
			rows[idx] = 0;
			once_stack.pop_back();
		}
	}
}

Rectanglef Animation::get_rec() const noexcept {
	Animation_Sheet& sheet = asset::Store.get_animation(sheet_key);

	size_t idx = once_stack.empty() ? running_idx : once_stack.back();

	auto& current_time  = rows[idx];
	auto& current_entry = sheet.entries[idx];

	auto index = (size_t)((current_time / current_entry.time) * current_entry.count);

	return {
		(float)(current_entry.start.x + index) / sheet.col,
		(float)(current_entry.start.y)         / sheet.entries.size(),
		1.f          / sheet.col,
		1.f          / sheet.entries.size()
	};
}

void from_dyn_struct(const dyn_struct& str, Animation_Sheet& animation) noexcept {
	animation.entries.reserve(size(str));
	for (const auto& x : iterate_array(str)) {
		Animation_Sheet::Entry entry;
		entry.start = (Vector2u)x["start"];
		entry.count = (size_t)x["count"];
		entry.time = (float)x["time"];

		animation.col = std::max(entry.count, animation.col);
		animation.entries.push_back(entry);
	}
}
void to_dyn_struct(dyn_struct&, const Animation_Sheet&) noexcept {
	// >TODO(Tackwin);

	assert(false);
}


