#include "Collision.hpp"

#include "Level.hpp"

bool test(const Block& b, const Player& p) noexcept {
	if (b.pos.x < p.pos.x + p.size.x && p.pos.x < b.pos.x + b.size.x &&
		b.pos.y < p.pos.y + p.size.y && p.pos.y < b.pos.y + b.size.y) return true;
	return false;
}

bool test(const Block& b, const Vector2f& v) noexcept {
	return
		b.pos.x < v.x && v.x < b.pos.x + b.size.x &&
		b.pos.y < v.y && v.y < b.pos.y + b.size.y;
}
