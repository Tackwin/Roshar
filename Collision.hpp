#pragma once

struct Block;
struct Player;
struct Kill_Zone;
struct Prest_Source;
template<size_t D, typename T>
struct Vector;

[[nodiscard]] extern bool test(const Block& b, const Vector<2, float>& v) noexcept;
[[nodiscard]] extern bool test(const Block& b, const Player& v) noexcept;
[[nodiscard]] extern bool test(const Prest_Source& x, const Vector<2, float>& v) noexcept;
[[nodiscard]] extern bool test(const Prest_Source& x, const Player& p) noexcept;
[[nodiscard]] extern bool test(const Kill_Zone& x, const Vector<2, float>& v) noexcept;
[[nodiscard]] extern bool test(const Kill_Zone& x, const Player& p) noexcept;
