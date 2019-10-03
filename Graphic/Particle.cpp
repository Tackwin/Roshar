#include "Particle.hpp"

#include "Managers/AssetsManager.hpp"

#include <random>
#include <algorithm>

void Particle::render(render::Orders& orders) const noexcept {
	if (t <= 0) return;

	Rectanglef rec;
	rec.size = V2F(size);
	rec.setCenter(pos);
	orders.push_rec(rec, color);
}

void Particle::update(float dt) noexcept {
	if (gravity) speed += Vector2f{ 0, -Environment.gravity } * dt;
	pos += speed * dt;
	t -= dt;
}

Particle_Spot::Particle_Spot(asset::Key key) noexcept {
	system_key = key;
	auto& system = asset::Store.get_particle(system_key);
	constexpr auto rand = [](auto t) noexcept {
		thread_local std::default_random_engine eng{ SEED };
		std::uniform_real_distribution<double> unit{ -0.5, +0.5 };

		return (decltype(t))(t * unit(eng));
	};
	timers.resize(system.emitters.size());
	for (size_t i = 0; i < timers.size(); ++i) {
		timers[i].t = system.emitters[i].time + rand(system.emitters[i].time_range);
	}
}

bool Particle_Spot::is_valid() const noexcept {
	return asset::Store.particle_systems.count(system_key) > 0;
}

void Particle_Spot::update(float dt, std::vector<Particle>& particles) noexcept {
	const auto& system = asset::Store.get_particle(system_key);
	constexpr auto rand = [](auto t) noexcept {
		thread_local std::default_random_engine eng{ SEED };
		std::uniform_real_distribution<double> unit{ -0.5, +0.5 };

		return (decltype(t))(t * unit(eng));
	};

	timers.resize(system.emitters.size());

	for (size_t i = 0; i < timers.size(); ++i) {
		auto& timer = timers[i];
		auto& emitter = system.emitters[i];

		timer.dt_t -= dt;
		timer.t -= dt;

		if (timer.t > emitter.time && emitter.once) continue;
		if (timer.dt_t <= 0) {
			Particle p;

			p.color = to_rgba(emitter.color_hsva + rand(emitter.color_range));
			p.pos =
				pos + emitter.spawn_zone.center() + rand(emitter.spawn_zone.size) + emitter.offset;
			p.size = emitter.size + rand(emitter.size_range);
			p.speed = Vector2f::createUnitVector(
				(emitter.direction + rand(emitter.direction_range)) * 2 * PI
			) * (emitter.speed + rand(emitter.speed_range));
			p.t = emitter.life_time + rand(emitter.life_time_range);
			p.gravity = rand(1) + 0.5f < emitter.gravity_probability;

			particles.push_back(p);

			timer.dt_t += emitter.dt_time + rand(emitter.dt_time_range);
		}
	}

	timers.erase(
		std::remove_if( BEG_END(timers), [](const Timers& t) { return t.t <= 0; }), END(timers)
	);
}

void from_dyn_struct(const dyn_struct& str, Particle_System& system) noexcept {
	Particle_System::Emitter e;
	for (const auto& x : iterate_array(str["emitters"])) {
		e = {};
		e.offset              = (Vector2f)x["offset"];
		e.spawn_zone          = (Rectanglef)x["spawn_zone"];
		e.color_hsva          = (Vector4d)x["color_hsva"];
		e.color_range         = (Vector4d)x["color_range"];
		e.direction           = (float)x["direction"];
		e.direction_range     = (float)x["direction_range"];
		e.distribution_kind   = (decltype(e.distribution_kind))(int)x["distribution_kind"];
		e.speed               = (float)x["speed"];
		e.speed_range         = (float)x["speed_range"];
		e.gravity_probability = (float)x["gravity_probability"];
		e.size                = (float)x["size"];
		e.size_range          = (float)x["size_range"];
		e.dt_time             = (float)x["dt_time"];
		e.dt_time_range       = (float)x["dt_time_range"];
		e.time                = (float)x["time"];
		e.time_range          = (float)x["time_range"];
		e.life_time           = (float)x["life_time"];
		e.life_time_range     = (float)x["life_time_range"];
		e.once                = (bool)x["once"];

		switch (e.distribution_kind) {
			case decltype(e.distribution_kind)::Normal: break;
			case decltype(e.distribution_kind)::Uniform: {
				e.mu    = (float)x["mu"];
				e.sigma = (float)x["sigma"];
				break;
			}
		}
		system.emitters.push_back(e);
	}
}
void to_dyn_struct(dyn_struct&, const Particle_System&) noexcept {
	// >TODO(Tackwin): When i'm doing a in game particle editor.
}

