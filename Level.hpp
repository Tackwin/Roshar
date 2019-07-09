#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Math/Vector.hpp"

struct Block {
	Vector2f pos;
	Vector2f size;
	void render(sf::RenderTarget& target) noexcept;
};

struct Player {
	Vector2f pos;
	Vector2f size{ 50, 100 };
	Vector2f velocity;
	Vector2f forces;

	void render(sf::RenderTarget& target) noexcept;
};

struct Level {
	sf::View camera;

	std::vector<Block> blocks;
	Player player;

	Level() noexcept;
	Level(Level&) = default;
	Level& operator=(Level&) = default;

	void render(sf::RenderTarget& target) noexcept;
	void update(float dt) noexcept;
};
