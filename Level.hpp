#pragma once

#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Math/Vector.hpp"
#include "Memory/ValuePtr.hpp"
#include "dyn_struct.hpp"

struct Block {
	bool editor_selected{ false };

	Vector2f pos;
	Vector2f size;
	void render(sf::RenderTarget& target) noexcept;
};

struct Kill_Zone {
	bool editor_selected{ false };
	
	Vector2f pos;
	Vector2f size;
	void render(sf::RenderTarget& target) noexcept;
};

struct Dispenser {
	bool editor_selected{ false };
	Vector2f start_pos;
	Vector2f end_pos;

	float hz;
	float proj_r;
	float proj_speed;

	float timer{ 0 };

	void render(sf::RenderTarget& target) noexcept;
};

struct Projectile {
	Vector2f pos;
	Vector2f speed;

	std::weak_ptr<Dispenser> origin{};

	float r;

	void render(sf::RenderTarget& target) noexcept;
};

struct Prest_Source {
	static const inline auto Radius_Multiplier = 0.2f;

	bool editor_selected{ false };

	float prest;
	Vector2f pos;

	void render(sf::RenderTarget& target) noexcept;
};

struct Player {
	Vector2f pos;
	Vector2f size{ 0.5f, 1 };
	Vector2f velocity;
	Vector2f forces;

	float prest{ 0.f };

	bool floored{ false };

	void update(float dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;
};

struct Level {
	ValuePtr<Level> initial_level;

	std::vector<Block> blocks;
	std::vector<std::shared_ptr<Dispenser>> dispensers;
	std::vector<Kill_Zone> kill_zones;
	std::vector<Projectile> projectiles;
	std::vector<Prest_Source> prest_sources;

	float camera_speed{ 10 };
	float camera_idle_radius{ 0.3f };
	sf::View camera;

	Player player;

	double drag_time{ 0.0 };
	std::optional<Vector2f> start_drag;
	float drag_dead_zone{ 50 };

	std::vector<Vector2f> basic_bindings;

	bool started{ false };

	Level() noexcept;
	Level(const Level&) = default;
	Level& operator=(const Level&) = default;

	void render(sf::RenderTarget& target) noexcept;
	void update(float dt) noexcept;

private:
	std::optional<Vector2f> camera_target;

	void update_camera(float dt) noexcept;
	void retry() noexcept;
};

extern void from_dyn_struct(const dyn_struct& str, Dispenser& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Dispenser& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Block& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Block& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Kill_Zone& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Kill_Zone& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Prest_Source& prest) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Prest_Source& prest) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Level& level) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Level& level) noexcept;
