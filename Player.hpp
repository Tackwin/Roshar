#pragma once

#include <vector>
#include <unordered_set>

#include <SFML/Graphics.hpp>

#include "Managers/InputsManager.hpp"

#include "Math/Vector.hpp"

struct Player {
	Vector2f pos;
	Vector2f size{ 0.4f, 0.7f };
	Vector2f velocity;
	Vector2f forces;

	enum Dir {
		Right,
		Left,
		None
	};

	std::unordered_set<std::uint64_t> auto_binded_from_zones;
	std::vector<Vector2f> flat_velocities;
	
	struct {
		std::vector<Vector2f> basic_bindings;
	} own;
	struct {
		std::vector<Vector2f> basic_bindings;
		std::vector<std::uint64_t> zones_uuid;
	} forced;

	std::vector<std::vector<Vector2f>*> binding_origin_history;

	float prest{ 0.f };

	bool floored{ false };
	bool just_jumped{ false };

	static constexpr float Coyotee_Time = 0.1f;
	float coyotee_timer = Coyotee_Time;

	static constexpr float Preshot_Time = 0.1f;
	float preshot_timer = 0;

	static constexpr float Jump_Strength_Modifier_Time = 0.3f;
	float jump_strength_modifier_timer = 0;

	static constexpr float Speed_Up_Time = 0.5f;
	float speed_up_timer = 0;

	static constexpr float Speed_Down_Time = 0.1f;
	float speed_down_timer = 0;

	Dir last_dir{ None };



	void input(Input_Iterator this_record) noexcept;
	void update(float dt) noexcept;
	void render(sf::RenderTarget& target) const noexcept;

	void jump() noexcept;
	void maintain_jump() noexcept;
	void directional_up() noexcept;
	void start_move_sideway() noexcept;
	void stop_move_sideway() noexcept;
	void move_sideway(Dir dir) noexcept;
	void fall_back() noexcept;

	void add_own_binding(Vector2f x) noexcept;
	void add_forced_binding(Vector2f x, std::uint64_t id) noexcept;

	void clear_all_basic_bindings() noexcept;
};
