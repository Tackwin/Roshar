#pragma once

#include <unordered_map>

#include "dyn_struct.hpp"

struct Profile {
	struct Time_Score {
		float best;
		float last;
	};
	std::unordered_map<std::string, Time_Score> best_time;
};


extern void to_dyn_struct(dyn_struct& str, const Profile& profile) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Profile& profile) noexcept;
