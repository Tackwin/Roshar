#pragma once

#include "Graphics.hpp"
#include "Math/Rectangle.hpp"
#include "Assets.hpp"

#include <vector>
#include <optional>

struct dyn_struct;

struct Particle {
	Vector2f pos;
	Vector4d color;
	float size;
	Vector2f speed;
	bool gravity;

	float t;

	void update(float dt) noexcept;
	void render(render::Orders& orders) const noexcept;
};

struct Particle_Spot {
	asset::Key system_key;
	Vector2f pos;

	struct Timers {
		float dt_t = 0.f;
		float t = 0.f;
	};

	std::vector<Timers> timers;

	Particle_Spot(asset::Key key) noexcept;

	bool is_valid() const noexcept;

	void update(float dt, std::vector<Particle>& particles) noexcept;
};

struct Particle_System {
	struct Emitter {
		Vector2f offset;

		Rectanglef spawn_zone;

		float direction;
		float direction_range;

		float speed;
		float speed_range;

		float gravity_probability;

		Vector4d color_hsva;
		Vector4d color_range;

		float size;
		float size_range;

		float dt_time;
		float dt_time_range;

		float time;
		float time_range;

		float life_time;
		float life_time_range;

		bool once;

		enum class Distribution_Type {
			Normal = 0,
			Uniform = 1,
			Count
		} distribution_kind;

		union {
			struct {
				float sigma;
				float mu;
			};
		};
	};

	std::vector<Emitter> emitters;
};

void from_dyn_struct(const dyn_struct& str, Particle_System& system) noexcept;
void to_dyn_struct(dyn_struct& str, const Particle_System& system) noexcept;

