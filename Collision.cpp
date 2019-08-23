#include "Collision.hpp"

#include "Level.hpp"

#include "Math/Circle.hpp"
#include "Math/Rectangle.hpp"
#include "Math/Vector.hpp"

bool test(const Rock& r, const Rectangle_t<float>& x) noexcept {
	return is_in(x, Circlef{ .c = r.pos,.r = r.r });
}

bool test(const Block& b, const Rock& x) noexcept {
	return is_in(Rectanglef(b.pos, b.size), Circlef{ .c = x.pos,.r = x.r });
}

bool test(const Dry_Zone& b, const Rock& x) noexcept {
	return is_in(b.rec, Circlef{ .c = x.pos,.r = x.r });
}

bool test(const Key_Item& b, const Rectangle_t<float>& x) noexcept {
	return x.in(b.pos);
}

bool test(const Block& b, const Player& p) noexcept {
	if (b.pos.x < p.pos.x + p.size.x && p.pos.x < b.pos.x + b.size.x &&
		b.pos.y < p.pos.y + p.size.y && p.pos.y < b.pos.y + b.size.y) return true;
	return false;
}

bool test(const Block& b, const Rectanglef& r) noexcept {
	return Rectangle_t{ b.pos, b.size }.intersect(r);
}

bool test(const Block& b, const Vector2f& v) noexcept {
	return
		b.pos.x < v.x && v.x < b.pos.x + b.size.x &&
		b.pos.y < v.y && v.y < b.pos.y + b.size.y;
}

bool test(const Kill_Zone& b, const Player& p) noexcept {
	if (b.pos.x < p.pos.x + p.size.x && p.pos.x < b.pos.x + b.size.x &&
		b.pos.y < p.pos.y + p.size.y && p.pos.y < b.pos.y + b.size.y) return true;
	return false;
}

bool test(const Kill_Zone& b, const Rectanglef& r) noexcept {
	return Rectangle_t{ b.pos, b.size }.intersect(r);
}

bool test(const Dry_Zone& b, const Player& p) noexcept {
	return b.rec.intersect({ p.pos,p.size });
}

bool test(const Dry_Zone& b, const Rectanglef& r) noexcept {
	return b.rec.intersect(r);
}

bool test(const Kill_Zone& b, const Vector2f& v) noexcept {
	return
		b.pos.x < v.x && v.x < b.pos.x + b.size.x &&
		b.pos.y < v.y && v.y < b.pos.y + b.size.y;
}

bool test(const Prest_Source& b, const Player& p) noexcept {
	Circlef c;
	c.c = b.pos;
	c.r = std::sqrt(b.prest) * Prest_Source::Radius_Multiplier;
	return is_in(Rectangle_t<float>(p.pos, p.size), c);
}

bool test(const Prest_Source& b, const Rectanglef& p) noexcept {
	Circlef c;
	c.c = b.pos;
	c.r = std::sqrt(b.prest) * Prest_Source::Radius_Multiplier;
	return is_in(Rectangle_t<float>(p.pos, p.size), c);
}

bool test(const Prest_Source& b, const Vector2f& p) noexcept {
	auto mult = Prest_Source::Radius_Multiplier * Prest_Source::Radius_Multiplier;
	//     (b.pos - p.pos).length2() < sqrt(b.prest)² * Prest_Source::Radius_Multiplier²
	return (b.pos - p).length2() < b.prest * mult;
}

bool test(const Auto_Binding_Zone& b, const Rectangle_t<float>& x) noexcept {
	return b.rec.intersect(x);
}
bool test(const Friction_Zone& b, const Rectangle_t<float>& x) noexcept {
	return b.rec.intersect(x);
}

bool test(const Projectile& x, const Player& p) noexcept {
	Circlef c;
	c.c = x.pos;
	c.r = x.r;
	return is_in(Rectangle_t<float>(p.pos, p.size), c);
}


bool test(const Dispenser& x, const Vector2f& p) noexcept {
	Circlef c;
	c.c = x.start_pos;
	c.r = x.proj_r;
	if (is_in(p, c)) return true;
	c.c = x.end_pos;
	return is_in(p, c);
}
bool test(const Dispenser& x, const Projectile& p) noexcept {
	Circlef c;
	c.c = x.end_pos;
	c.r = p.r;
	return is_in(p.pos, c);
}

bool test(const Dispenser& x, const Rectanglef& r) noexcept {
	Circlef c;
	c.c = x.start_pos;
	c.r = x.proj_r;
	return is_in(r, c);
}

bool test(const Next_Zone& x, const Rectangle_t<float>& r) noexcept {
	return r.intersect({ x.pos, x.size });
}
bool test(const Next_Zone& x, const Player& p) noexcept {
	return Rectanglef{ x.pos, x.size }.intersect({ p.pos, p.size });
}

std::optional<Vector2f> get_next_velocity(
	const Circlef& circle, const Vector2f current_velocity, const Rectanglef& rec, float dt
) noexcept {
	if (!is_in(rec, circle)) return std::nullopt;
	Vector2f c = circle.c - current_velocity * dt;

	Vector2f p = c + Vector2f{0, 1};
	if (is_in(rec.botLeft(), circle)) {
		p = rec.botLeft();
	}
	else if (is_in(rec.botRight(), circle)) {
		p = rec.botRight();
	}
	else if (is_in(rec.topLeft(), circle)) {
		p = rec.topLeft();
	}
	else if (is_in(rec.topRight(), circle)) {
		p = rec.topRight();
	}
	else if (c.x < rec.x || rec.x + rec.w < c.x) {
		p = c + Vector2f{ 1, 0 };
	}
	//else if (circle.c.y < rec.y || rec.y + rec.h < circle.c.y)

	Vector2f dt_vec = p - c;
	Vector2f dtp = dt_vec.rotate90();

	return current_velocity.projectTo(dtp);
}

bool test(const Trigger_Zone& b, const Rectangle_t<float>& x) noexcept {
	return b.rec.intersect(x);
}
bool test(const Door& b, const Rectangle_t<float>& x) noexcept {
	return b.rec.intersect(x);
}
bool test(const Door& b, const Player& x) noexcept {
	return b.closed && b.rec.intersect(Rectanglef{ x.pos, x.size });
}
bool test(const Decor_Sprite& b, const Player& x) noexcept {
	return b.rec.intersect(Rectanglef{ x.pos, x.size });
}
