#include "Profile.hpp"

void to_dyn_struct(dyn_struct& str, const Profile& profile) noexcept {
	str = dyn_struct::structure_t{};

	for (auto& [k, v] : profile.best_time) {
		str[k]["best"] = v.best;
		str[k]["last"] = v.last;
	}
}

void from_dyn_struct(const dyn_struct& str, Profile& profile) noexcept {
	for (const auto& [k, v] : iterate_structure(str)) {
		profile[k].best = v["best"];
		profile[k].last = v["last"];
	}
}


