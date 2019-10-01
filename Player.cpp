#include "Player.hpp"

#include <assert.h>

#include <imgui.h>

#include "Game.hpp"
#include "Assets.hpp"
#include "Collision.hpp"
#include "Math/Circle.hpp"

constexpr size_t Idle_Animation = 0;
constexpr size_t Run_Animation = 1;

Animation& get_animation() noexcept;

void Player::input(Input_Iterator this_record) noexcept {
	auto prev_motion = wanted_motion;
	wanted_drag = this_record->right_joystick;
	wanted_motion = this_record->left_joystick;
	mouse_screen_pos = this_record->mouse_screen_pos;
	mouse_world_pos = this_record->mouse_world_pos(game->current_level.camera);

	want_slow = this_record->is_pressed(Keyboard::LSHIFT);
	want_slow |= this_record->left_trigger == 1;
	want_to_go = Dir::None;

	if (wanted_drag.length() >= 1) {
		auto candidate = wanted_drag.angleX();

		// The first time the player go a direction we register that correctly.
		if (!started_joystick_drag) {
			start_drag_time = game->timeshots;
			started_joystick_drag = true;
		}

		// When the player didn't let go but changed orientation we wait a little time
		// to be sure he actually wanted to change.
		if (candidate != right_joystick_drag && candidate != controller_drag_candidate) {
			controller_drag_candidate = candidate;
			controller_binding_candidate_time = Controller_Binding_Candidate_Timer;
		}
		else if (controller_binding_candidate_time < 0.f) {
			right_joystick_drag = controller_drag_candidate;
			right_joystick_timer_to_zero = Right_Joystick_Time_To_Zero;
		}

	}
	if (started_joystick_drag && wanted_drag.length() < 1) {
		if (right_joystick_timer_to_zero > 0) end_drag(right_joystick_drag);
		started_joystick_drag = false;
		right_joystick_drag = 0;
	}

	if (prev_motion.length2() > wanted_motion.length2()) {
		stop_move_sideway();
	}

	if (wanted_motion.x != 0) {
		if (prev_motion.x > 0 != wanted_motion.x > 0) {
			start_move_sideway();
		}
		move_factor = std::abs(wanted_motion.x);
		auto dir_to_go = wanted_motion.x > 0 ? Player::Right : Player::Left;
		move_sideway(dir_to_go);
	}
	else {
		if (this_record->is_pressed(Keyboard::Q)) {
			if (this_record->is_just_pressed(Keyboard::Q)) start_move_sideway();
			move_sideway(Player::Left);
		}
		else if (this_record->is_just_released(Keyboard::Q)) {
			stop_move_sideway();
		}
		if (this_record->is_pressed(Keyboard::D)) {
			if (this_record->is_just_pressed(Keyboard::D)) start_move_sideway();
			move_sideway(Player::Right);
		}
		else if (this_record->is_just_released(Keyboard::D)) {
			stop_move_sideway();
		}
	}


	if (this_record->is_pressed(Keyboard::S) || wanted_motion.y <= -.5) {
		want_to_go = Dir::Down;
		fall_back();
	}
	else {
		falling_back = false;
	}
	if (this_record->is_just_pressed(Mouse::Left)) {
		start_drag();
	}
	else if (!this_record->is_pressed(Mouse::Left)) {
		dragging = false;
	}

	grappling  = this_record->is_pressed(Joystick::LB);
	grappling |= this_record->is_pressed(Mouse::Right);
	grappling &= !cant_grap;
	
	if (this_record->is_pressed(Keyboard::Z) || wanted_motion.y >= .5) want_to_go = Dir::Up;

	if (
		this_record->is_just_pressed(Keyboard::Space) ||
		this_record->is_just_pressed(Joystick::A) ||
		this_record->is_just_pressed(Joystick::RB)
	) {
		if (floored || coyotee_timer > 0 || grappled) {
			jump();
		}
		else {
			preshot_timer = Preshot_Time;
		}
	}
	if (jump_strength_modifier_timer > 0) {
		if (
			this_record->is_pressed(Keyboard::Space) ||
			this_record->is_pressed(Joystick::A) ||
			this_record->is_pressed(Joystick::RB)
		) maintain_jump();

		if (this_record->is_pressed(Keyboard::Z) || wanted_motion.y >= .5) directional_up();
	}

	if (this_record->is_just_pressed(Joystick::B)) {
		controller_clear_timer = Controller_Clear_Time;
	}
	if (
		this_record->is_just_pressed(Mouse::Right) ||
		(this_record->is_pressed(Joystick::B) && controller_clear_timer < .0f)
	) {
		own.basic_bindings.clear();
		for (size_t i = binding_origin_history.size() - 1; i + 1 > 0; --i) {
			if (binding_origin_history[i] == &own.basic_bindings) {
				binding_origin_history.erase(BEG(binding_origin_history) + i);
			}
		}
	}
	auto keyboard_cancel =
		this_record->is_pressed(Keyboard::LCTRL) && this_record->is_just_pressed(Keyboard::Z);
	auto controller_cancel = this_record->is_just_pressed(Joystick::B);
	auto back_not_empty =
		!binding_origin_history.empty() && !binding_origin_history.back()->empty();

	if ((keyboard_cancel || controller_cancel) && back_not_empty) {
		binding_origin_history.back()->pop_back();
		binding_origin_history.pop_back();

		// When we cancel the previous binding we could cancel a forced binding.
		// So we need to make sure to remove our id bag so that if we re enter a auto binding zone
		//  we would be binded again.
		for (size_t i = forced.zones_uuid.size() - 1; i + 1 > forced.basic_bindings.size(); --i) {
			auto_binded_from_zones.erase(forced.zones_uuid.back());
			forced.zones_uuid.pop_back();
		}
	}
}

void Player::update(float dt) noexcept {
	coyotee_timer -= dt;
	preshot_timer -= dt;
	speed_up_timer -= dt;
	speed_down_timer -= dt;
	controller_clear_timer -= dt;
	saturated_touch_last_time -= dt;
	jump_strength_modifier_timer -= dt;
	right_joystick_timer_to_zero -= dt;
	controller_binding_candidate_time -= dt;
	jump_cant_grap_timer -= dt;

	get_animation().update(animation_idx, dt);

	if (jump_cant_grap_timer < 0.f) {
		cant_grap = false;
	}

	drag_indicator_t += dt * 5;
	drag_indicator_t = std::fmodf(drag_indicator_t, 1.f);

	if (!moving && last_dir != None && speed_down_timer > 0) {
		auto slow_down = std::clamp(speed_down_timer / Speed_Down_Time, 0.f, 1.f);
		if (last_dir == Left) slow_down *= -5;
		else slow_down *= 5;
		slow_down *= move_factor;
		slow_down *= want_slow ? Slow_Factor : 1;

		flat_velocities.push_back({ slow_down, 0 });
	}

	if (dragging) on_drag();

	if (!grappled) {
		for (const auto& x : own.basic_bindings) forces += x * Environment.gravity;
		for (const auto& x : forced.basic_bindings) forces += x * Environment.gravity;
		forces.y -= Environment.gravity;

		velocity += forces * dt;
		velocity *= std::powf(Environment.drag * (floored ? 0.1f : 1.f), dt);
	}

	if (moving && !floored) {
		float lambda = std::expf(-dt * velocity.length2());
		velocity_from_jump.x *= lambda;
	}

	forces = {};

	moving = false;
	if (grappled) {
		auto mass = 1;
		prest -= dt * mass / 2;
	}
	grappling &= prest > 0;
	grappled &= grappling;
	grappled &= want_to_go == Dir::None;
}

void Player::render(render::Orders& target) const noexcept {
	auto rec = Rectanglef{ pos, size };
	if (last_dir == Dir::Left) rec = rec.flip_x();

	target.push_sprite(rec, asset::Texture_Id::Guy_Sheet, get_animation().get_rec(animation_idx));

	render_bindings(target);
}

void Player::render_bindings(render::Orders& orders) const noexcept {
	auto center = pos + size / 2;
	auto body_texture = asset::Texture_Id::Basic_Binding_Indicator_Body;
	auto head_texture = asset::Texture_Id::Basic_Binding_Indicator_Head;

	Rectanglef sprite_sheet_rect = { .0f, 0.2f * std::roundf(drag_indicator_t / 0.2f), 1.f, .2f };

	auto f = [&](auto a, auto x) {
		orders.push_sprite(
			a,
			{ x.length() * .9f, x.length() * .1f },
			body_texture,
			sprite_sheet_rect,
			{ .0f, .5f },
			(float)x.angleX()
		);
		orders.push_sprite(
			a + x,
			{ x.length() * .1f, 0.15f * x.length() },
			head_texture,
			sprite_sheet_rect,
			{ 1.f, .5f },
			(float)x.angleX()
		);
	};

	for (auto& x : own.basic_bindings) f(center, x);
	for (auto& x : forced.basic_bindings) f(center, x);

	if (dragging) {
		auto prest_gathered =
			std::ceilf((float)(Environment.gather_speed * (game->timeshots - start_drag_time))) *
			Environment.gather_step;

		if (saturated_touch_last_time <= 0) {
			prest_gathered = std::min(prest_gathered, prest);
		}

		prest_gathered *= .1f;

		orders.push_circle(prest_gathered, mouse_world_pos, { 0, 1, 0, 1 });
		f(
			IM::applyInverseView(game->current_level.camera, start_drag_pos),
			mouse_world_pos - IM::applyInverseView(game->current_level.camera, start_drag_pos)
		);
	}
	if (started_joystick_drag) {
		auto prest_gathered =
			std::ceilf((float)(Environment.gather_speed * (game->timeshots - start_drag_time))) *
			Environment.gather_step;

		if (saturated_touch_last_time <= 0) {
			prest_gathered = std::min(prest_gathered, prest);
		}

		prest_gathered *= .1f;
		orders.push_circle(prest_gathered, pos + size / 2, { 0, 1, 0, 1 });
	}

}


void Player::clear_all_basic_bindings() noexcept {
	own.basic_bindings.clear();
	forced.basic_bindings.clear();
	forced.zones_uuid.clear();
	auto_binded_from_zones.clear();
}

void Player::start_move_sideway() noexcept {
	animation_idx = Run_Animation;
	if (speed_up_timer <= 0) speed_up_timer = Speed_Up_Time;
	speed_down_timer = 0.f;
	move_factor = 1.f;
}
void Player::stop_move_sideway() noexcept {
	speed_down_timer = Speed_Down_Time;
	animation_idx = Idle_Animation;
}
void Player::move_sideway(Player::Dir dir) noexcept {
	want_to_go = dir;

	auto top_speed = std::sqrtf(std::clamp(1.f - speed_up_timer / Speed_Up_Time, 0.f, 1.f));

	if (dir == Right) top_speed *= 5;
	else if (dir == Left) top_speed *= -5;
	else assert(false); // Logic error.

	top_speed *= move_factor * (want_slow ? Slow_Factor : 1);
	flat_velocities.push_back({ top_speed, 0 });
	last_dir = dir;
	moving = true;
	grappling = false;
}

void Player::jump() noexcept {
	Vector2f normal = { 0, 1 };

	if (grappled) {
		normal = grappling_normal;
		switch (want_to_go) {
		case Dir::Left:
			normal += Vector2f{ -1, 0 };
			break;
		case Dir::Right:
			normal += Vector2f{ +1, 0 };
			break;
		case Dir::Up:
			normal += Vector2f{ 0, +1 };
			break;
		case Dir::Down:
			normal += Vector2f{ 0, -1 };
			break;
		default:
			break;
		}
	}
	normal.normalize();

	if (!falling_back) {
		velocity_from_jump += 1.5f * normal;
	}
	velocity_from_jump += 5.f * normal;
	jump_strength_modifier_timer = Jump_Strength_Modifier_Time;
	just_jumped = true;
	coyotee_timer = 0.f;
	grappled = false;
	grappling = false;
	cant_grap = true;
	jump_cant_grap_timer = Jump_Cant_Grap_Time;
}
void Player::maintain_jump() noexcept {
	if (!falling_back) flat_velocities.push_back({ 0, 1.5f });
}
void Player::directional_up() noexcept {
	if (!falling_back) flat_velocities.push_back({ 0, 0.75 });
}
void Player::fall_back() noexcept {
	falling_back = true;
	if (!floored) {
		flat_velocities.push_back({ 0, -1 });
	}
}

void Player::add_forced_binding(Vector2f x, std::uint64_t id) noexcept {
	binding_origin_history.push_back(&forced.basic_bindings);
	forced.basic_bindings.push_back(x);
	forced.zones_uuid.push_back(id);
	auto_binded_from_zones.insert(id);
}

void Player::add_own_binding(Vector2f x) noexcept {
	binding_origin_history.push_back(&own.basic_bindings);
	own.basic_bindings.push_back(x);
}

void Player::start_drag() noexcept {
	dragging = true;
	start_drag_pos = mouse_screen_pos;
	start_drag_time = game->timeshots;

	for (const auto& x : game->current_level.rocks) {
		auto range = Environment.binding_range * Environment.binding_range;
		if (
			is_in(mouse_world_pos, { x.pos, x.r }) &&
			(x.pos - (pos + size / 2)).length2() <= range
		) {
			dragged_rock = x.running_id;
			return;
		}
	}
}

void Player::on_drag() noexcept {
	auto dt = mouse_screen_pos - start_drag_pos;
	if (dt.length2() < Drag_Dead_Zone * Drag_Dead_Zone) return;
	defer{
		dragged_rock = 0;
		dragging = false;
	};

	end_drag(dt.angleX());
}

void Player::end_drag(double angle) noexcept{
	auto discrete_angle = angle;
	auto angle_step = 2 * PI / Environment.drag_angle_step;
	discrete_angle = angle_step * std::round(discrete_angle / angle_step);

	auto unit = Vector2f::createUnitVector(discrete_angle);
	auto prest_gathered =
		(int)(std::ceil(Environment.gather_speed * (game->timeshots - start_drag_time))) *
		Environment.gather_step;

	if (saturated_touch_last_time <= 0) {
		prest_gathered = std::min(prest_gathered, prest);
	}

	if (prest_gathered == 0) return;
	prest -= prest_gathered;
	prest = std::max(0.f, prest);

	auto amount = unit * prest_gathered;

	if (dragged_rock) return game->current_level.bind_rock(dragged_rock, amount);
	if (game->current_level.focused_rock) {
		auto id = game->current_level.rocks[*game->current_level.focused_rock].running_id;
		return game->current_level.bind_rock(id, amount);
	}

	add_own_binding(unit * prest_gathered);
}

void Player::clear_movement_x() noexcept {
	velocity.x = 0;
	velocity_from_jump.x = 0;
	forces.x = 0;
}
void Player::clear_movement_y() noexcept {
	velocity.y = 0;
	velocity_from_jump.y = 0;
	forces.y = 0;
}

Vector2f Player::get_final_velocity() noexcept {
	return velocity + velocity_from_jump + get_direct_control_velocity();
}
void Player::apply_friction(float factor) noexcept {
	velocity *= factor;
	velocity_from_jump *= factor;
}
Vector2f Player::get_direct_control_velocity() noexcept {
	Vector2f flat_sum = { 0, 0 };
	for (const auto& x : flat_velocities) flat_sum += x;
	return flat_sum;
}

Animation& get_animation() noexcept {
	return asset::Store.get_animation(asset::Animation_Id::Guy);
}
