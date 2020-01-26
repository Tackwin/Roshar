#include "Screens/Settings.hpp"

#include "Managers/AssetsManager.hpp"

#include "Game.hpp"

void Settings_Screen::input(IM::Input_Iterator it) noexcept {
	last_input = it;
}

void Settings_Screen::update(float dt) noexcept {
	for (auto& [_, x] : button_states){
		if (x.hovering) x.time_since_hover += dt;
		else x.time_since_hover = 0;
	}
	for (auto& [_, x] : key_prompts){
	}
}

void Settings_Screen::render(render::Orders& target) noexcept {
	auto& bindings = game->control_bindings;
	
	target.late_push_view(
		{ 0.f, 0.f, (float)Environment.window_width, (float)Environment.window_height }
	);
	defer { target.late_pop_view(); };
	
	go_back = button(target, {50, 50}, "Back", 42, {0, 0});

	auto y_step = 50;
	Vector2f pos = {Environment.window_width * 0.33f, Environment.window_height * 0.75f};
	pos.y -= y_step;

	key_prompt(target, pos, "Jump", 21, &bindings.jump);     pos.y -= y_step;
	key_prompt(target, pos, "Up", 21, &bindings.up);         pos.y -= y_step;
	key_prompt(target, pos, "Left", 21, &bindings.left);     pos.y -= y_step;
	key_prompt(target, pos, "Down", 21, &bindings.down);     pos.y -= y_step;
	key_prompt(target, pos, "Right", 21, &bindings.right);   pos.y -= y_step;
	key_prompt(target, pos, "Slow", 21, &bindings.slow);     pos.y -= y_step;
	key_prompt(target, pos, "Grap", 21, &bindings.grap);     pos.y -= y_step;
	key_prompt(target, pos, "Cancel", 21, &bindings.cancel); pos.y -= y_step;
	key_prompt(target, pos, "Clear", 21, &bindings.clear);   pos.y -= y_step;
}

bool Settings_Screen::button(
	render::Orders& target, Vector2f pos, std::string label, float size, Vector2f origin
) noexcept {
	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);

	size += 0.25f * size * xstd::map_rp_to_01(10 * button_states[label].time_since_hover);

	auto rec = Rectanglef{pos, 1.3f * font.compute_size(label, size)};
	rec.pos.x -= origin.x * rec.size.x;
	rec.pos.y -= origin.y * rec.size.y;
	target.late_push_text(pos, asset::Font_Id::Consolas, label, size, origin);

	if (!IM::iterator_is_valid(last_input)) return false;

	bool hovering = rec.in(last_input->mouse_screen_pos);
	bool clicking = last_input->is_just_pressed(Mouse::Left);

	button_states[label].hovering = hovering;

	return hovering && clicking;
}

Keyboard::Key Settings_Screen::key_prompt(
	render::Orders& target,
	Vector2f pos,
	std::string label,
	float size,
	Control_Bindings::Action* binded
) noexcept {
	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);
	auto& it = key_prompts[label];

	auto rec = Rectanglef{pos, font.compute_size(label, size)};
	if (IM::iterator_is_valid(last_input)) {
		it.hovering = rec.in(last_input->mouse_screen_pos);
		if (last_input->is_just_pressed(Mouse::Left)) it.focused = it.hovering; 

		auto last_key = last_input->last_key_pressed();
		if (it.focused && last_key) {
			binded->key = *last_key;
			it.focused = false;
		}
		
		auto last_controller = last_input->last_contoller_pressed();
		if (it.focused && last_controller) {
			binded->controller = *last_controller;
			it.focused = false;
		}
	}

	using Style = render::Text_Info::Style;
	std::uint32_t style = Style::Normal;
	if (it.hovering) style |= Style::Underline;

	auto& text_size = asset::Store.get_font(asset::Font_Id::Consolas).compute_size(label, size);

	auto back_rec = Rectanglef{
		Environment.window_width * .1f,
		pos.y - 2,
		Environment.window_width * .8f,
		text_size.y + 4
	};
	target.late_push_rec(back_rec, {0.07, 0.13, 0.21, 0.25});

	if (it.focused) target.late_push_rec({pos, text_size}, {1, 1, 1, 0.25});
	target.late_push_text(pos, asset::Font_Id::Consolas, label, size, {0, 0}, style);

	pos.x += text_size.x + size / 2;
	label.clear();
	if (binded->key) label += to_string(*binded->key) + ", ";
	if (binded->controller) label += to_string(*binded->controller) + ", ";
	if (label.size()) {
		label.pop_back();
		label.pop_back();
	}

	target.late_push_text(pos, asset::Font_Id::Consolas, label, size, {0, 0});

	return Keyboard::Key::Space;
}

Keyboard::Key Settings_Screen::key_prompt(
	render::Orders& target,
	Vector2f pos,
	std::string label,
	float size,
	Keyboard::Key* binded
) noexcept {
	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);
	auto& it = key_prompts[label];

	auto rec = Rectanglef{pos, font.compute_size(label, size)};
	if (IM::iterator_is_valid(last_input)) {
		it.hovering = rec.in(last_input->mouse_screen_pos);
		if (last_input->is_just_pressed(Mouse::Left)) it.focused = it.hovering; 

		auto last_key = last_input->last_key_pressed();
		if (it.focused && last_key) {
			*binded = *last_key;
			it.focused = false;
		}
	}

	using Style = render::Text_Info::Style;
	std::uint32_t style = Style::Normal;
	if (it.hovering) style |= Style::Underline;



	auto& text_size = asset::Store.get_font(asset::Font_Id::Consolas).compute_size(label, size);

	auto back_rec = Rectanglef{
		Environment.window_width * .1f,
		pos.y - 2,
		Environment.window_width * .8f,
		text_size.y + 4
	};
	target.late_push_rec(back_rec, {0.07, 0.13, 0.21, 0.25});
	if (it.focused) {
		target.late_push_rec({pos, text_size}, {1, 1, 1, 0.25});
	}

	target.late_push_text(
		pos, asset::Font_Id::Consolas, label, size, {0, 0}, style
	);

	pos.x += text_size.x + size / 2;
	label.clear();
	if (binded) label += to_string(*binded) + ", ";
	if (label.size()) {
		label.pop_back();
		label.pop_back();
	}

	target.late_push_text(pos, asset::Font_Id::Consolas, label, size, {0, 0});

	return Keyboard::Key::Space;
}