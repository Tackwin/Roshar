#pragma once
#include "Vector.hpp"

template<typename T>
struct Rectangle_t {
	enum class Side {
		Left = 0,
		Right,
		Top,
		Bot,
		None,
		Size
	};

	constexpr static Vector2<T> get_normal(Side s) noexcept {
		switch (s) {
		case Side::Left: return { -1, 0 };
		case Side::Top: return { 0, 1 };
		case Side::Right: return { 1, 0 };
		case Side::Bot: return { 0, -1 };
		default: return { 0, 0 };
		}
	}

	constexpr static Vector2<T> get_tangent(Side s) noexcept {
		switch (s) {
		case Side::Left: return { 1, 0 };
		case Side::Top: return { 0, 1 };
		case Side::Right: return { 1, 0 };
		case Side::Bot: return { 0, 1 };
		default: return { 0, 0 };
		}
	}

	constexpr static Rectangle_t<T> centered(Vector2<T> p, Vector2<T> size) noexcept {
		Rectangle_t<T> result;
		result.size = size;
		result.setCenter(p);
		return result;
	}

	union {
		struct {
			Vector<2, T> pos;
			Vector<2, T> size;
		};
		struct {
			T x;
			T y;
			T w;
			T h;
		};
	};

	constexpr Rectangle_t() {}
	constexpr Rectangle_t(T x, T y, T w, T h) noexcept : x(x), y(y), w(w), h(h) {
		if constexpr (!std::is_unsigned_v<T>) {
			if (this->size.x < 0) {
				this->pos.x += this->size.x;
				this->size.x = -this->size.x;
			}
			if (this->size.y < 0) {
				this->pos.y += this->size.y;
				this->size.y = -this->size.y;
			}
		}
	}
	constexpr Rectangle_t(const Vector<2, T>& pos, const Vector<2, T>& size) :
		pos(pos),
		size(size)
	{
		if constexpr (!std::is_unsigned_v<T>) {
			if (this->size.x < 0) {
				this->pos.x += this->size.x;
				this->size.x = -this->size.x;
			}
			if (this->size.y < 0) {
				this->pos.y += this->size.y;
				this->size.y = -this->size.y;
			}
		}
	}

	Rectangle_t(const Rectangle_t&) = default;
	Rectangle_t& operator=(const Rectangle_t&) = default;
	Rectangle_t(Rectangle_t&&) = default;
	Rectangle_t& operator=(Rectangle_t&&) = default;

	template<typename U> explicit operator Rectangle_t<U>() const noexcept {
		return {
			Vector2<U>{(U)x, (U)y},
			Vector2<U>{(U)w, (U)h}
		};
	}

	bool intersect(const Rectangle_t<T>& other) const {
		return !(
				pos.x + size.x < other.pos.x || pos.x > other.pos.x + other.size.x ||
				pos.y + size.y < other.pos.y || pos.y > other.pos.y + other.size.y
			);
	}

	constexpr Side intersect_side(const Rectangle_t<T>& other) const noexcept {
		if (!intersect(other)) return Side::None;

		auto left = x - (other.x + other.w); left *= left;
		auto right = (x + w) - other.x; right *= right;
		auto top = (y + h) - other.y; top *= top;
		auto bot = y - (other.y + other.h); bot *= bot;

		if (left == std::min<T>({ left, right, top, bot })) return Side::Left;
		else if (right == std::min<T>({ left, right, top, bot })) return Side::Right;
		else if (top == std::min<T>({ left, right, top, bot })) return Side::Top;
		else  return Side::Bot;
	}

	// works only for top down y
	bool isOnTopOf(Rectangle_t<T> other) const {
		T distEdgeToEdge = std::max(other.x + other.w - x, x + w - other.x);
		T sumOfWidth = w + other.w;

		return y < other.y && distEdgeToEdge < sumOfWidth;
	}
	bool isFullyOnTopOf(Rectangle_t<T> other, T tolerance = FLT_EPSILON) const noexcept {
		return y + h < other.y + tolerance;
	}

	// works only for top down y
	bool isOnBotOf(Rectangle_t<T> other) const {
		T distEdgeToEdge = std::max(other.x + other.w - x, x + w - other.x);
		T sumOfWidth = w + other.w;

		return y > other.y && distEdgeToEdge < sumOfWidth;
	}

	Rectangle_t<T> zoom(float factor) noexcept {
		return {
			center() - size * factor * 0.5,
			size * factor
		};
	}

	Vector<2, T> center() const {
		return pos + size / 2;
	}

	void setCenter(Vector2<T> vec) noexcept {
		pos = vec - size / 2.0;
	}

	T bot() const {
		return pos.y + size.y;
	}

	std::tuple<
		Rectangle_t<T>, Rectangle_t<T>, Rectangle_t<T>, Rectangle_t<T>
	> divide() const noexcept {
		return {
			Rectangle_t<T>{pos, size / 2},
			Rectangle_t<T>{ {pos.x + size.x / 2, pos.y}, size / 2 },
			Rectangle_t<T>{ {pos.x, pos.y + size.y / 2}, size / 2 },
			Rectangle_t<T>{pos + size / 2, size / 2},
		};
	}

	// >TODO: implement this, for now we treat this as if it were the support
	// of a circle of radius max(w, h) / 2
	Vector2<T> support(T a, T d) const noexcept {
		return Vector2<T>::createUnitVector((double)a) * (d + std::max({ w, h }) / 2);
	}

	Vector2<T> topLeft() const noexcept {
		return { x, y + h };
	}
	Vector2<T> topRight() const noexcept {
		return pos + size;
	}
	Vector2<T> botLeft() const noexcept {
		return pos;
	}
	Vector2<T> botRight() const noexcept {
		return { x + w, y };
	}

	Rectangle_t<T> fitUpRatio(double ratio) const noexcept {
		if (w > h) {
			return { pos,{ w, (T)(w / ratio) } };
		}
		else {
			return { pos,{ (T)(h * ratio), h } };
		}
	}
	Rectangle_t<T> fitDownRatio(double ratio) const noexcept {
		if (w < h) {
			return { pos,{ w, (T)(w / ratio) } };
		}
		else {
			return { pos,{ (T)(h * ratio), h } };
		}
	}

	Rectangle_t<T> restrict_in(Rectangle_t<T> area) const noexcept {
		Rectangle_t<T> result = *this;

		if (w > area.w) {
			result.x = area.center().x - result.size.x / 2;
		}
		else {
			// there is a reason it's made like that
			// if for instance area is a narrow (in height) space, then *this can jump up
			// and down if instead of if (result.x ...) it were if (x ...)
			if (x + w > area.x + area.w) {
				result.x = area.x + area.w - w;
			}
			if (result.x < area.x) {
				result.x = area.x;
			}
		}

		if (h > area.h) {
			result.y = area.center().y - result.size.y / 2;
		} else {
			if (y + h > area.y + area.h) {
				result.y = area.y + area.h - h;
			}
			if (result.y < area.y) {
				result.y = area.y;
			}
		}
		return result;
	}

	T area() const noexcept {
		return w * h;
	}

	template<typename U>
	bool in(const Vector<2, U>& p) const {
		return p.inRect(pos, size);
	}

	static Rectangle_t<T> hull(std::vector<Rectangle_t<T>> recs) noexcept {
		Rectangle_t<T> hull = recs[0];

		for (auto rec : recs) {
			hull.x = std::min(rec.x, hull.x);
			hull.y = std::min(rec.y, hull.y);
		}
		for (auto rec : recs) {
			hull.w = std::max(rec.x + rec.w, hull.w + hull.x) - hull.x;
			hull.h = std::max(rec.y + rec.h, hull.h + hull.y) - hull.y;
		}
		return hull;
	}

};

using Rectanglef = Rectangle_t<float>;
using Rectanglei = Rectangle_t<int>;
using Rectangleu = Rectangle_t<size_t>;

struct dyn_struct;
template<typename T>
void from_dyn_struct(const dyn_struct& str, Rectangle_t<T>& x) noexcept {
	x.x = (T)str[0];
	x.y = (T)str[1];
	x.w = (T)str[2];
	x.h = (T)str[3];
}
template<typename T>
void to_dyn_struct(dyn_struct& str, const Rectangle_t<T>& x) noexcept {
	str = dyn_struct::array_t{};
	str.push_back(x.x);
	str.push_back(x.y);
	str.push_back(x.w);
	str.push_back(x.h);
}

template<typename T>
double dist_to2(const Vector<2, T>& vec, const Rectangle_t<T> & rec) noexcept {
	if (
		(vec.x < rec.x || vec.x > rec.x + rec.w) &&
		(rec.y < vec.y && vec.y < rec.y + rec.h)
	) {
		return (rec.x - vec.x) * (rec.x - vec.x);
	}
	if (
		(vec.y < rec.y || vec.y > rec.y + rec.h) &&
		(rec.x < vec.x && vec.x < rec.x + rec.w)
	) {
		return (rec.y - vec.y) * (rec.y - vec.y);
	}

	Vector<2, T> dt;

	if (vec.x < rec.x) {
		if (vec.y < rec.y)   dt = (vec - Vector<2, T>{rec.x, rec.y});
		else                 dt = (vec - Vector<2, T>{rec.x, rec.y + rec.h});
	}
	else {
		if (vec.y < rec.y)   dt = (vec - Vector<2, T>{rec.x + rec.w, rec.y});
		else                 dt = (vec - Vector<2, T>{rec.x + rec.w, rec.y + rec.h});
	}

	return dt.length2();
}