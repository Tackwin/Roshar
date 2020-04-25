#pragma once

#include <vector>
#include <unordered_set>

#include "Managers/InputsManager.hpp"
#include "Assets.hpp"

#include "Math/Vector.hpp"

#include "Graphic/Animation.hpp"
#include "Graphic/Graphics.hpp"


struct Player {
	struct Graphic_State {
		Rectanglef world_rec;
		Rectanglef texture_rec;
	};

	Rectanglef hitbox{ 0, 0, .4f, .7f };
	Vector2f sprite_size{ 0.4f, 0.8f };
	Vector2f velocity;
	Vector2f forces;

	Vector2f wanted_motion{0, 0};
	Vector2f wanted_drag{ 0, 0 };

	Vector2f grappling_normal{ 0, 0 };

	enum Dir {
		Right,
		Left,
		Up,
		Down,
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


	std::vector<std::uint64_t> own_keys;

	std::vector<std::vector<Vector2f>*> binding_origin_history;

	float prest{ 0.f };

	bool floored{ false };
	bool just_floored{ false };
	bool just_jumped{ false };
	bool falling_back{ false };

	static constexpr float Saturated_Touch_Last_Time = 0.2f;
	float saturated_touch_last_time = 0;

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

	static constexpr float Controller_Binding_Candidate_Timer = 0.05f;
	float controller_binding_candidate_time = 0.f;

	Dir last_dir{ None };

	bool grappled{ false };
	bool grappling{ false };

	Graphic_State graphic_state;

	void input(Input_Iterator this_record) noexcept;
	void update(float dt) noexcept;
	void render(render::Orders& target) const noexcept;
	void render(render::Orders& target, Graphic_State graphic_state) const noexcept;

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

	void clear_movement_x() noexcept;
	void clear_movement_y() noexcept;

	Vector2f get_final_velocity() const noexcept;
	Vector2f get_direct_control_velocity() const noexcept;
	void apply_friction(float factor) noexcept;

	Graphic_State get_graphic_state() const noexcept;
private:
	static constexpr float Slow_Factor{ 0.5f };
	static constexpr float Drag_Dead_Zone{ 200.f };
	std::uint64_t dragged_rock{ 0 };

	Vector2f mouse_screen_pos;
	Vector2f mouse_world_pos;

	Dir want_to_go{ None };

	float move_factor{ 1.f };
	bool want_slow{ false };
	bool dragging{ false };
	bool moving{ false };
	Vector2f start_drag_pos;
	Vector2f start_drag_world_pos;
	double start_drag_time;

	double right_joystick_drag;
	double controller_drag_candidate;
	bool started_joystick_drag{ false };

	static constexpr float Right_Joystick_Time_To_Zero = { 0.05f };
	float right_joystick_timer_to_zero{ 0.f };

	static constexpr float Controller_Clear_Time = { 0.2f };
	float controller_clear_timer{ 0.f };

	static constexpr float Jump_Cant_Grap_Time = { .5f };
	float jump_cant_grap_timer{ 0.f };

	float particle_foot_timer{ 0.f };
	bool run_particle{ true };

	float drag_indicator_t{ 0.f };

	bool cant_grap{ false };

	Vector2f velocity_from_jump{};

	Animation animation{ asset::Animation_Id::Guy };

	void render_bindings(render::Orders& orders) const noexcept;

	void start_drag() noexcept;
	void on_drag() noexcept;
	void end_drag(double angle) noexcept;
};
