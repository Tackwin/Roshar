#pragma once

#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Math/Rectangle.hpp"
#include "Math/Vector.hpp"
#include "Memory/ValuePtr.hpp"
#include "dyn_struct.hpp"

struct Block {
	bool editor_selected{ false };

	Vector2f pos;
	Vector2f size;
	void render(sf::RenderTarget& target) const noexcept;
};

struct Kill_Zone {
	bool editor_selected{ false };
	
	Vector2f pos;
	Vector2f size;
	void render(sf::RenderTarget& target) const noexcept;
};

struct Next_Zone {
	bool editor_selected{ false };
	
	std::string next_level;

	Vector2f pos;
	Vector2f size;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Trigger_Zone {
	bool editor_selected{ false };

	std::uint64_t id;
	Rectanglef rec;
	bool triggered{ false };

	void render(sf::RenderTarget& target) const noexcept;
};

struct Door {
	bool editor_selected{ false };
	
	Rectanglef rec;

	bool closed{ true };

	std::vector<std::uint64_t> must_triggered;
	std::vector<std::uint64_t> mustnt_triggered;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Dry_Zone {
	bool editor_selected{ false };

	Rectanglef rec;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Dispenser {
	bool editor_selected{ false };
	Vector2f start_pos;
	Vector2f end_pos;

	float hz;
	float proj_r;
	float proj_speed;

	float timer{ 0 };
	float offset_timer{ 0 };

	void render(sf::RenderTarget& target) const noexcept;

	void set_start_timer() noexcept;
};

struct Projectile {
	Vector2f pos;
	Vector2f speed;

	Vector2f end_pos;

	float r;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Prest_Source {
	static const inline auto Radius_Multiplier = 0.2f;

	bool editor_selected{ false };

	float prest;
	Vector2f pos;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Rock {
	bool editor_selected{ false };

	float r;
	Vector2f pos;
	Vector2f velocity;

	std::vector<Vector2f> bindings;
	float mass;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Player {
	Vector2f pos;
	Vector2f size{ 0.5f, 1 };
	Vector2f velocity;
	Vector2f forces;

	std::vector<Vector2f> flat_velocities;
	float prest{ 0.f };

	bool floored{ false };

	void update(float dt) noexcept;
	void render(sf::RenderTarget& target) const noexcept;
};

struct Level {
	// >SEE:
	// >DEBUG:
	struct Debug_Vector {
		Vector2f a;
		Vector2f b;
	};
	std::vector<Debug_Vector> debug_vectors;

	ValuePtr<Level> initial_level;

	std::vector<Rock> rocks;
	std::vector<Door> doors;
	std::vector<Block> blocks;
	std::vector<Dry_Zone> dry_zones;
	std::vector<Kill_Zone> kill_zones;
	std::vector<Next_Zone> next_zones;
	std::vector<Dispenser> dispensers;
	std::vector<Projectile> projectiles;
	std::vector<Trigger_Zone> trigger_zones;
	std::vector<Prest_Source> prest_sources;

	std::vector<Vector2f> markers;

	float camera_speed{ 10 };
	float camera_idle_radius{ 0.3f };
	sf::View camera;

	Player player;

	double drag_time{ 0.0 };
	std::optional<Vector2f> start_drag;
	float drag_dead_zone{ 50 };
	size_t rock_dragging_i{ 0 };

	std::vector<Vector2f> basic_bindings;

	static constexpr float Input_Active_Time = 0.5f;
	float input_active_timer = Input_Active_Time;

	static constexpr float Camera_Fade_Time = 0.5f;
	float camera_fade_out_timer = 0;
	float camera_fade_in_timer = 0;

	bool in_editor{ false };

	Level() noexcept;
	Level(const Level&) = default;
	Level& operator=(const Level&) = default;

	void render(sf::RenderTarget& target) const noexcept;
	void update(float dt) noexcept;

	void pause() noexcept;
	void resume() noexcept;

private:
	std::uint64_t speedrun_clock_start;

	std::optional<Vector2f> camera_target;

	bool test_input(float dt) noexcept;

	void test_collisions(float dt, Player previous_player) noexcept;

	void mouse_start_drag() noexcept;
	void mouse_on_drag() noexcept;

	void update_camera(float dt) noexcept;
	void retry() noexcept;
	void die() noexcept;
};

extern void from_dyn_struct(const dyn_struct& str, Dispenser& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Dispenser& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Dry_Zone& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Dry_Zone& x) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Next_Zone& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Next_Zone& x) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Block& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Block& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Kill_Zone& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Kill_Zone& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Prest_Source& prest) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Prest_Source& prest) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Rock& r) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Rock& r) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Trigger_Zone& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Trigger_Zone& x) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Door& d) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Door& d) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Level& level) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Level& level) noexcept;
