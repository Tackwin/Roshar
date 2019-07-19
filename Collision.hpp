#pragma once

struct Block;
struct Player;
struct Kill_Zone;
struct Dispenser;
struct Projectile;
struct Prest_Source;
struct Next_Zone;
struct Dry_Zone;

template<size_t D, typename T>
struct Vector;
template<typename T>
struct Rectangle;

[[nodiscard]] extern bool test(const Block& b, const Vector<2, float>& v) noexcept;
[[nodiscard]] extern bool test(const Block& b, const Rectangle<float>& r) noexcept;
[[nodiscard]] extern bool test(const Block& b, const Player& v) noexcept;
[[nodiscard]] extern bool test(const Prest_Source& x, const Vector<2, float>& v) noexcept;
[[nodiscard]] extern bool test(const Prest_Source& x, const Rectangle<float>& r) noexcept;
[[nodiscard]] extern bool test(const Prest_Source& x, const Player& p) noexcept;
[[nodiscard]] extern bool test(const Next_Zone& x, const Rectangle<float>& r) noexcept;
[[nodiscard]] extern bool test(const Next_Zone& x, const Player& p) noexcept;
[[nodiscard]] extern bool test(const Dry_Zone& x, const Rectangle<float>& r) noexcept;
[[nodiscard]] extern bool test(const Dry_Zone& x, const Player& p) noexcept;
[[nodiscard]] extern bool test(const Kill_Zone& x, const Rectangle<float>& r) noexcept;
[[nodiscard]] extern bool test(const Kill_Zone& x, const Vector<2, float>& v) noexcept;
[[nodiscard]] extern bool test(const Kill_Zone& x, const Player& p) noexcept;
[[nodiscard]] extern bool test(const Projectile& x, const Player& p) noexcept;
[[nodiscard]] extern bool test(const Dispenser& x, const Vector<2, float>& p) noexcept;
[[nodiscard]] extern bool test(const Dispenser& x, const Rectangle<float>& r) noexcept;
[[nodiscard]] extern bool test(const Dispenser& x, const Projectile& p) noexcept;
