#pragma once

#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Math/Vector.hpp"

struct Block {
	bool editor_selected{ false };

	Vector2f pos;
	Vector2f size;
	void render(sf::RenderTarget& target) noexcept;
};

struct Prest_Source {
	float prest{};
};

struct Player {
	Vector2f pos;
	Vector2f size{ 50, 100 };
	Vector2f velocity;
	Vector2f forces;


	void update(float dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;
};

struct Level {
	std::vector<Block> blocks;
	float camera_speed{ 1000 };
	float camera_idle_radius{ 30 };
	sf::View camera;

	Player player;

	double drag_time{ 0.0 };
	std::optional<Vector2f> start_drag;
	float drag_dead_zone{ 50 };

	std::vector<Vector2f> basic_bindings;

	Level() noexcept;
	Level(Level&) = default;
	Level& operator=(Level&) = default;

	void render(sf::RenderTarget& target) noexcept;
	void update(float dt) noexcept;

private:
	std::optional<Vector2f> camera_target;

	void update_camera(float dt) noexcept;
};
