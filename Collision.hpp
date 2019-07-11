#pragma once

struct Block;
struct Player;
template<size_t D, typename T>
struct Vector;

[[nodiscard]] extern bool test(const Block& b, const Vector<2, float>& v) noexcept;
[[nodiscard]] extern bool test(const Block& b, const Player& v) noexcept;
