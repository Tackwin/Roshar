#include "Profile.hpp"

void to_dyn_struct(dyn_struct& str, const Profile& profile) noexcept {
	str = dyn_struct::structure_t{};

	str["name"] = profile.name;

	str["times"] = dyn_struct::structure_t{};

	for (auto& [k, v] : profile.best_time) {
		str["times"][k]["best"] = v.best;
		str["times"][k]["last"] = v.last;
	}

	str["bindings"] = profile.bindings;
	str["current_level"] = profile.current_level;
}

void from_dyn_struct(const dyn_struct& str, Profile& profile) noexcept {
	const auto& times = *at(str, "times");
	for (const auto& [k, v] : iterate_structure(times)) {
		profile.best_time[k].best = (float)*at(v, "best");
		profile.best_time[k].last = (float)*at(v, "last");
	}

	profile.name = (std::string)str["name"];
	profile.bindings = (Control_Bindings)str["bindings"];
	profile.current_level = (std::string)str["current_level"];
}

void to_dyn_struct(dyn_struct& str, const Control_Bindings& bindings) noexcept {
	auto action = [&](const char* name, Control_Bindings::Action act) {
		if (act.key) str[name]["key"] = (size_t)*act.key;
		if (act.button) str[name]["button"] = (size_t)*act.button;
		if (act.controller) str[name]["controller"] = (size_t)*act.controller;
	};
	#define X(x) action(#x, bindings.x);
	X(jump);
	X(up);
	X(left);
	X(right);
	X(down);
	X(grap);
	X(cancel);
	X(clear);
	X(cancel);
	#undef X

	str["slow"] = (size_t)bindings.slow;
	str["start_drag"] = (size_t)bindings.start_drag;
	str["clear_bindings"] = (size_t)bindings.clear_bindings;
}

void from_dyn_struct(const dyn_struct& str, Control_Bindings& bindings) noexcept {
	auto action = [&](const char* name) {
		Control_Bindings::Action act;

		if (has(str[name], "key")) act.key = (Keyboard::Key)(size_t)str[name]["key"];
		if (has(str[name], "button")) act.button = (Mouse::Button)(size_t)str[name]["button"];
		if (has(str[name], "controller"))
			act.controller = (Joystick::Button)(size_t)str[name]["controller"];

		return act;
	};
	#define X(x) bindings.x = action(#x);
	X(jump);
	X(up);
	X(left);
	X(right);
	X(down);
	X(grap);
	X(cancel);
	X(clear);
	X(cancel);
	#undef X

	bindings.slow = (Keyboard::Key)(size_t)str["slow"];
	bindings.start_drag = (Mouse::Button)(size_t)str["start_drag"];
	bindings.clear_bindings = (Mouse::Button)(size_t)str["clear_bindings"];

}


