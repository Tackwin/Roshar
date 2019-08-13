#include "Player.hpp"

#include <assert.h>

void Player::input(Input_Iterator this_record) noexcept {
	if (this_record->is_pressed(sf::Keyboard::Q)) {
		if (this_record->is_just_pressed(sf::Keyboard::Q)) start_move_sideway();
		move_sideway(Player::Right);
	}
	else if (this_record->is_just_released(sf::Keyboard::Q)) {
		stop_move_sideway();
	}
	if (this_record->is_pressed(sf::Keyboard::D)) {
		if (this_record->is_just_pressed(sf::Keyboard::D)) start_move_sideway();
		move_sideway(Player::Left);
	}
	else if (this_record->is_just_released(sf::Keyboard::D)) {
		stop_move_sideway();
	}
	if (this_record->is_pressed(sf::Keyboard::S)) {
		fall_back();
	}
	if (this_record->is_just_pressed(sf::Keyboard::Space)) {
		if (floored || coyotee_timer > 0) {
			jump();
		}
		else {
			preshot_timer = Preshot_Time;
		}
	}
	if (jump_strength_modifier_timer > 0) {
		if (this_record->is_pressed(sf::Keyboard::Space)) maintain_jump();
		if (this_record->is_pressed(sf::Keyboard::Z)) directional_up();
	}
	if (this_record->is_just_pressed(sf::Mouse::Right)) own.basic_bindings.clear();
	if (
		this_record->is_pressed(sf::Keyboard::LControl) &&
		this_record->is_just_pressed(sf::Keyboard::Z) &&
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
	jump_strength_modifier_timer -= dt;

	if (last_dir != None && speed_down_timer > 0) {
		auto slow_down = std::clamp(speed_down_timer / Speed_Down_Time, 0.f, 1.f);
		if (last_dir == Right) slow_down *= -5;
		else slow_down *= 5;

		flat_velocities.push_back({ slow_down, 0 });
	}

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
	if (dir == Right) top_speed *= -5;
	else if (dir == Left) top_speed *= 5;
	else assert(false); // Logic error.
	flat_velocities.push_back({ top_speed, 0 });
	last_dir = dir;
}

void Player::jump() noexcept {
	velocity += Vector2f{ 0, 6.5 };
	jump_strength_modifier_timer = Jump_Strength_Modifier_Time;
}
void Player::maintain_jump() noexcept {
	flat_velocities.push_back({ 0, 1.5 });
}
void Player::directional_up() noexcept {
	flat_velocities.push_back({ 0, 0.75 });
}
void Player::fall_back() noexcept {
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
