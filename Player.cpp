#include "Player.hpp"

#include <assert.h>

#include "Game.hpp"
#include "Collision.hpp"
#include "Math/Circle.hpp"

void Player::input(Input_Iterator this_record) noexcept {
	auto prev_motion = wanted_motion;
	wanted_motion = this_record->joystick_axis;
	mouse_screen_pos = this_record->mouse_screen_pos;
	mouse_world_pos = this_record->mouse_world_pos(game->camera);

	auto dead_zone = this_record->Joystick_Dead_Zone * this_record->Joystick_Dead_Zone;
	if (wanted_motion.length2() > dead_zone) {
		if (std::signbit(prev_motion.x) != std::signbit(wanted_motion.x)) {
			start_move_sideway();
		}
		
		if (std::abs(wanted_motion.x) > this_record->Joystick_Dead_Zone) {
			move_sideway(wanted_motion.x > 0 ? Player::Right : Player::Left);
		}

	} else if (prev_motion.length2() > dead_zone) {
		stop_move_sideway();
	}

	if (this_record->is_pressed(sf::Keyboard::Q)) {
		if (this_record->is_just_pressed(sf::Keyboard::Q)) start_move_sideway();
		move_sideway(Player::Left);
	}
	else if (this_record->is_just_released(sf::Keyboard::Q)) {
		stop_move_sideway();
	}
	if (this_record->is_pressed(sf::Keyboard::D)) {
		if (this_record->is_just_pressed(sf::Keyboard::D)) start_move_sideway();
		move_sideway(Player::Right);
	}
	else if (this_record->is_just_released(sf::Keyboard::D)) {
		stop_move_sideway();
	}
	if (
		this_record->is_pressed(sf::Keyboard::S) ||
		wanted_motion.y < -1 * this_record->Joystick_Dead_Zone
	) {
		fall_back();
	}
	else {
		falling_back = false;
	}
	if (this_record->is_just_pressed(sf::Mouse::Left)) {
		start_drag();
	}
	else if (!this_record->is_pressed(sf::Mouse::Left)) {
		dragging = false;
	}

	if (
		this_record->is_just_pressed(sf::Keyboard::Space) ||
		this_record->is_just_pressed(Joystick_Button::A)
	) {
		if (floored || coyotee_timer > 0) {
			jump();
		}
		else {
			preshot_timer = Preshot_Time;
		}
	}
	if (jump_strength_modifier_timer > 0) {
		if (
			this_record->is_pressed(sf::Keyboard::Space) ||
			this_record->is_pressed(Joystick_Button::A)
		) maintain_jump();
		if (
			this_record->is_pressed(sf::Keyboard::Z) ||
			wanted_motion.y > this_record->Joystick_Dead_Zone
		) directional_up();
	}
	if (
		this_record->is_just_pressed(sf::Mouse::Right) ||
		this_record->is_just_pressed(Joystick_Button::B)
	) {
		own.basic_bindings.clear();
		for (size_t i = binding_origin_history.size() - 1; i + 1 > 0; --i) {
			if (binding_origin_history[i] == &own.basic_bindings) {
				binding_origin_history.erase(BEG(binding_origin_history) + i);
			}
		}
	}
		if (
			(
				(
					this_record->is_pressed(sf::Keyboard::LControl) &&
					this_record->is_just_pressed(sf::Keyboard::Z)
				) ||
				this_record->is_just_pressed(Joystick_Button::RB)
			) &&
			!binding_origin_history.empty()
		) {
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
	saturated_touch_last_time -= dt;
	jump_strength_modifier_timer -= dt;

	if (last_dir != None && speed_down_timer > 0) {
		auto slow_down = std::clamp(speed_down_timer / Speed_Down_Time, 0.f, 1.f);
		if (last_dir == Left) slow_down *= -5;
		else slow_down *= 5;

		flat_velocities.push_back({ slow_down, 0 });
	}

	if (dragging) on_drag();

	for (const auto& x : own.basic_bindings) forces += x * Environment.gravity;
	for (const auto& x : forced.basic_bindings) forces += x * Environment.gravity;
	forces.y -= Environment.gravity;

	velocity += forces * dt;
	velocity *= std::powf(Environment.drag * (floored ? 0.1f : 1.f), dt);

	forces = {};
}

void Player::render(sf::RenderTarget& target) const noexcept {
	sf::RectangleShape shape(size);
	shape.setOutlineColor(Vector4f{ 1.0, 0.0, 0.0, 1.0 });
	shape.setOutlineThickness(0.01f);
	shape.setPosition(pos);
	shape.setFillColor(sf::Color::Cyan);
	target.draw(shape);

	for (auto& binding : own.basic_bindings) {
		Vector2f::renderArrow(
			target,
			pos + size / 2,
			pos + size / 2 + binding,
			{ 1, 0, 1, 1 },
			{ 1, 0, 1, 1 }
		);
	}
	for (auto& binding : forced.basic_bindings) {
		Vector2f::renderArrow(
			target,
			pos + size / 2,
			pos + size / 2 + binding,
			{ 0, 1, 1, 1 },
			{ 0, 1, 1, 1 }
		);
	}

	if (dragging) {
		auto prest_gathered =
			std::ceilf((float)(Environment.gather_speed * (game->timeshots - start_drag_time))) *
			Environment.gather_step;

		if (saturated_touch_last_time <= 0) {
			prest_gathered = std::min(prest_gathered, prest);
		}

		prest_gathered *= .1f;

		sf::CircleShape shape;
		shape.setRadius(prest_gathered);
		shape.setOrigin(prest_gathered, prest_gathered);
		shape.setPosition(mouse_world_pos);
		shape.setFillColor(sf::Color::Green);
		shape.setOutlineThickness(0.01f);
		shape.setOutlineColor(sf::Color::White);
		target.draw(shape);

		Vector2f::renderLine(
			target,
			target.mapPixelToCoords(start_drag_pos),
			mouse_world_pos,
			{ 0, 1, 0, 1 }
		);
	}

}

void Player::clear_all_basic_bindings() noexcept {
	own.basic_bindings.clear();
	forced.basic_bindings.clear();
	forced.zones_uuid.clear();
	auto_binded_from_zones.clear();
}

void Player::start_move_sideway() noexcept {
	if (speed_up_timer <= 0) speed_up_timer = Speed_Up_Time;
}
void Player::stop_move_sideway() noexcept {
	speed_down_timer = Speed_Down_Time;
}
void Player::move_sideway(Player::Dir dir) noexcept {
	auto top_speed = std::sqrtf(std::clamp(1.f - speed_up_timer / Speed_Up_Time, 0.f, 1.f));
	if (dir == Right) top_speed *= 5;
	else if (dir == Left) top_speed *= -5;
	else assert(false); // Logic error.
	flat_velocities.push_back({ top_speed, 0 });
	last_dir = dir;
}

void Player::jump() noexcept {
	if (!falling_back) {
		velocity += Vector2f{ 0, 1.5f };
	}
	velocity += Vector2f{ 0, 5.f };
	jump_strength_modifier_timer = Jump_Strength_Modifier_Time;
	just_jumped = true;
	coyotee_timer = 0.f;
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
	dt.y *= -1;
	if (dt.length2() < Drag_Dead_Zone * Drag_Dead_Zone) return;
	defer{
		dragged_rock = 0;
		dragging = false;
	};

	auto discrete_angle = dt.angleX();
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

	if (dragged_rock) return game->current_level.bind_rock(dragged_rock, unit * prest_gathered);
	add_own_binding(unit * prest_gathered);
}
