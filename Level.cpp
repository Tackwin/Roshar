#include "Level.hpp"

#include "Managers/InputsManager.hpp"

void Block::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.0, 0.0, 0.0, 1.0 });
	shape.setOutlineThickness(1);
	shape.setPosition(pos);
	target.draw(shape);
}

void Player::render(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.0, 0.0, 0.0, 1.0 });
	shape.setOutlineThickness(1);
	shape.setPosition(pos);
	shape.setFillColor(sf::Color::Cyan);
	target.draw(shape);
}

void Level::render(sf::RenderTarget& target) noexcept {
	target.setView(camera);
	for (auto& x : blocks) x.render(target);
	player.render(target);
}

bool blockPlayer(Block b, Player p) noexcept {

	if (b.pos.x < p.pos.x + p.size.x && p.pos.x < b.pos.x + b.size.x &&
		b.pos.y < p.pos.y + p.size.y && p.pos.y < b.pos.y + b.size.y) return true;
	return false;
}

bool blockPlayer(std::vector<Block>& blocks, Player player) noexcept {
	for (const auto& b : blocks) if (blockPlayer(b, player)) return true;
	return false;
}

Level::Level() noexcept {
	camera.setSize({ 1280, -720 });
}

void Level::update(float dt) noexcept {
	auto prev_player_pos = player.pos;

	auto prevPlayer = player;
	if (IM::isKeyPressed(sf::Keyboard::Q)) {
		player.pos.x -= dt * 500;
	}
	if (IM::isKeyPressed(sf::Keyboard::D)) {
		player.pos.x += dt * 500;
	}
	if (IM::isKeyJustPressed(sf::Keyboard::Space)) {
		player.velocity.y += 750;
	}
	if (IM::isMousePressed(sf::Mouse::Left)) {
		player.forces.y += 981;
	}

	player.velocity.y -= 981 * dt;
	player.velocity += player.forces * dt;
	player.velocity *= std::powf(0.4f, dt);
	player.forces = {};

	player.pos.x += player.velocity.x * dt;
	if (blockPlayer(blocks, player)) {
		player.pos.x = prevPlayer.pos.x;
		player.velocity.x = 0;
	}
	player.pos.y += player.velocity.y * dt;
	if (blockPlayer(blocks, player)) {
		player.pos.y = prevPlayer.pos.y;
		player.velocity.y = 0;
	}
}
