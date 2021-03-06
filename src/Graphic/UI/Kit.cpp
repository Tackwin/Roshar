#include "Kit.hpp"

#include "Managers/AssetsManager.hpp"

#include <unordered_map>

static kit::State state;

extern kit::State& kit::get_state() noexcept { return state; }

void kit::key_prompt(std::string label, float size, Keyboard::Key& binded) noexcept {
	Control_Bindings::Action act;
	act.key = binded;
	key_prompt(label, size, act);
	binded = *act.key;
}

void kit::key_prompt(std::string label, float size, Control_Bindings::Action& binded) noexcept {
	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);
	auto& it = state.key_prompt_sates[label];

	auto pos = state.current_pos;

	auto rec = Rectanglef{pos, font.compute_size(label, size)};
	if (IM::iterator_is_valid(state.last_input)) {
		it.hovering = rec.in(state.last_input->mouse_screen_pos);
		if (state.last_input->is_just_pressed(Mouse::Left)) it.focused = it.hovering; 

		auto last_key = state.last_input->last_key_pressed();
		if (binded.key && it.focused && last_key) {
			binded.key = *last_key;
			it.focused = false;
		}
		
		auto last_controller = state.last_input->last_contoller_pressed();
		if (binded.controller && it.focused && last_controller) {
			binded.controller = *last_controller;
			it.focused = false;
		}
	}

	using Style = render::Text_Info::Style;
	std::uint32_t style = Style::Normal;
	if (it.hovering) style |= Style::Underline;

	auto text_size = asset::Store.get_font(asset::Font_Id::Consolas).compute_size(label, size);

	auto back_rec = Rectanglef{
		Environment.window_width * .1f,
		pos.y - 2,
		Environment.window_width * .8f,
		text_size.y + 4
	};
	state.orders.late_push_rec(back_rec, {0.07, 0.13, 0.21, 0.25});

	if (it.focused) state.orders.late_push_rec({pos, text_size}, {1, 1, 1, 0.25});
	state.orders.late_push_text(pos, asset::Font_Id::Consolas, label, size, {0, 0}, style);

	pos.x += text_size.x + size / 2;
	label.clear();
	if (binded.key) label += to_string(*binded.key) + ", ";
	if (binded.controller) label += to_string(*binded.controller) + ", ";
	if (label.size()) {
		label.pop_back();
		label.pop_back();
	}

	state.orders.late_push_text(pos, asset::Font_Id::Consolas, label, size, {0, 0});

	state.current_pos.y += text_size.y + state.button_margin;
}


bool kit::button(std::string label, float size) noexcept {
	auto pos = state.current_pos;
	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);

	size += 0.25f * size * xstd::map_rp_to_01(10 * state.button_states[label].time_hovered);

	auto rec = Rectanglef{pos, 1.3f * font.compute_size(label, size)};
	rec.pos.x -= rec.size.x * state.current_origin.x;
	rec.pos.y -= rec.size.y * state.current_origin.y;
	state.orders.late_push_text(
		pos, asset::Font_Id::Consolas, label, size, state.current_origin, 0, state.current_color
	);

	if (!IM::iterator_is_valid(state.last_input)) return false;

	bool hovering = rec.in(state.last_input->mouse_screen_pos);
	bool clicking = state.last_input->is_just_pressed(Mouse::Left);

	state.button_states[label].hovering = hovering;

	return hovering && clicking;
}

void kit::input(IM::Input_Iterator it) noexcept {
	state.last_input = it;
	state.orders.clear();
	state.orders.late_push_view(
		{ 0.f, 0.f, (float)Environment.window_width, (float)Environment.window_height }
	);
}

void kit::update(float dt) noexcept {
	State::Card_State::Card_Id = 0;
	for (auto& [_, x] : state.button_states) {
		if (x.hovering) x.time_hovered += dt;
		else x.time_hovered = 0;
	}
	for (auto& [_, x] : state.card_states) {
		if (x.hovering) x.time_hovered += dt;
		else x.time_hovered = 0;
	}
	for (auto& [_, x] : state.input_text_states) {
		if (x.hovering) x.time_hovered += dt;
		else x.time_hovered = 0;
	}
}

void kit::render(render::Orders& target) noexcept {
	target.append(state.orders);
}

kit::Card_Response kit::card(Profile& profile) noexcept {
	kit::Card_Response res;
	auto& card_state = state.card_states[State::Card_State::Card_Id++];

	auto pos = state.current_pos;
	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);
	auto name = profile.name;
	auto size = font.compute_size(name, state.card_title_size);

	pos.x += state.card_margin;

	size.y += 2 * state.card_padding;
	size.x = std::max(state.card_min_size.x, size.x);
	size.y = std::max(state.card_min_size.y, size.y);

	pos.y += 0.25f * pos.y * xstd::map_rp_to_01(10 * card_state.time_hovered);
	Rectanglef rec = { pos, size };

	state.orders.late_push_rec(rec, state.card_color);
	state.orders.late_push_text(
		pos + Vector2f{ size.x / 2.f, size.y * .9f },
		asset::Font_Id::Consolas,
		name,
		state.card_title_size,
		{ 0.5, 1.f }
	);

	std::string uptime;
	uptime.resize(13);
	sprintf(uptime.data(), "Uptime %5.f", profile.uptime);
	state.orders.late_push_text(
		pos + Vector2f{ size.x / 2.f, size.y * .8f },
		asset::Font_Id::Consolas,
		uptime,
		state.card_uptime_size,
		{ 0.5, 1.f }
	);

	if (card_state.hovering) {
		auto temp_pos = state.current_pos;
		auto temp_color = state.current_color;
		defer {
			state.current_color = temp_color;
			state.current_pos = temp_pos;
		};
		state.current_color = {1, 1, 1, xstd::map_rp_to_01(10 * card_state.time_hovered)};
		state.current_pos = pos + Vector2f{ size.x / 2.f, size.y * .6f };
		auto button_size = 0.7f + 0.4f * xstd::map_rp_to_01(10 * card_state.time_hovered);
		if (button("Delete", state.card_delete_size * button_size)) {
			res.erase = true;
		}
	}


	pos.x += size.x + state.card_margin;
	state.current_pos.x = pos.x;

	if (IM::iterator_is_valid(state.last_input)) {
		card_state.hovering = rec.in(state.last_input->mouse_screen_pos);
		if (state.last_input->is_just_pressed(Mouse::Left))
			card_state.selected = card_state.hovering; 
	}
	

	res.enter = !res.erase && IM::iterator_is_valid(state.last_input) &&
		rec.in(state.last_input->mouse_screen_pos) &&
		state.last_input->is_just_pressed(Mouse::Left);
	return res;
}

std::optional<kit::State::Card_State> kit::card_plus() noexcept {
	auto& card_state = state.card_states[State::Card_State::Card_Id++];
	
	auto pos = state.current_pos;
	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);
	auto name = "New";
	auto size = font.compute_size(name, state.card_title_size);

	pos.x += state.card_margin;

	size.y += 2 * state.card_padding;
	size.x = std::max(state.card_min_size.x, size.x);
	size.y = std::max(state.card_min_size.y, size.y);

	pos.y += 0.25f * pos.y * xstd::map_rp_to_01(10 * card_state.time_hovered);
	Rectanglef rec = { pos, size };

	state.orders.late_push_rec(rec, state.card_color);
	if (card_state.selected) {
		auto temp_pos = state.current_pos;
		auto temp_ori = state.current_origin;
		defer {
			state.current_pos = temp_pos;
			state.current_origin = temp_ori;
		};
		state.current_pos = pos + Vector2f{ size.x / 2, size.y * .9f },
		state.current_origin = {.5f, 1.f};
		card_state.name = kit::text_input("Player Name", state.card_title_size);
	} else {
		state.orders.late_push_text(
			pos + Vector2f{ size.x / 2, size.y * .9f },
			asset::Font_Id::Consolas,
			name,
			state.card_title_size,
			{ 0.5f, 1.f }
		);
	}

	Rectanglef card_plus_rec;
	card_plus_rec.size = state.card_min_size / 2;
	card_plus_rec.h = card_plus_rec.w;
	card_plus_rec.x = rec.pos.x + rec.size.x / 2;
	card_plus_rec.y = rec.pos.y + rec.size.y / 3;
	state.orders.late_push_sprite(
		card_plus_rec,
		asset::Texture_Id::Card_Plus,
		{0, 0, 1, 1},
		{0.5f, 0.5f}
	);

	pos.x += size.x + state.card_margin;

	state.current_pos.x = pos.x;

	if (IM::iterator_is_valid(state.last_input)) {
		card_state.hovering = rec.in(state.last_input->mouse_screen_pos);
		if (state.last_input->is_just_pressed(Mouse::Left))
			card_state.selected = card_state.hovering; 
	}

	if (
		card_state.selected &&
		IM::iterator_is_valid(state.last_input) &&
		state.last_input->is_just_pressed(Keyboard::Return)
	) return card_state;
	else return std::nullopt;
}		

// >SEE: It seems that win32 has a feature to handle carets(text cursor) natively. I should use that
// i guess it would be usefull to the semantic of the program.
std::string kit::text_input(std::string def, float size) noexcept {
	auto& input_state = state.input_text_states[def];

	if (IM::iterator_is_valid(state.last_input)) {
		if (state.last_input->new_character != '\0') {
			switch (state.last_input->new_character)
			{
			case 0x08: // bakcspace
				if (!input_state.str.empty()) input_state.str.pop_back();
				input_state.cursor_pos -= input_state.cursor_pos > 0 ? 1 : 0;
				break;
			case 0x0A: //linefeed
			case 0x1B: //escape
			case 0x09: //tab
			case 0x0D: //carriage return
				break;
			default:
				input_state.str.insert(
					BEG(input_state.str) + input_state.cursor_pos, state.last_input->new_character
				);
				input_state.cursor_pos++;
				break;
			}
		}

		if (state.last_input->is_just_pressed(Keyboard::Left)) {
			printf("Left\n");
			input_state.cursor_pos -= (input_state.cursor_pos > 0) ? 1 : 0;
		}
		if (state.last_input->is_just_pressed(Keyboard::Right)) {
			printf("Right\n");
			input_state.cursor_pos += (input_state.cursor_pos < input_state.str.size()) ? 1 : 0;
		}
	}

	auto pos = state.current_pos;
	if (input_state.str.empty())
		state.orders.late_push_text(
			pos,
			asset::Font_Id::Consolas,
			def,
			size,
			state.current_origin,
			render::Text_Info::Style::Normal,
			{ 0.8, 0.8, 0.8, 0.8 }
		);
	else
		state.orders.late_push_text(
			pos, asset::Font_Id::Consolas, input_state.str, size, state.current_origin
		);

	auto displayed = input_state.str.empty() ? def : input_state.str;

	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);
	auto cursor_pos = font.compute_size(displayed.substr(0, input_state.cursor_pos), size);
	auto text_size = font.compute_size(displayed, size);

	Vector2f offset = {
		state.current_origin.x * text_size.x,
		state.current_origin.y * text_size.y,
	};

	state.orders.late_push_rec(
		{pos + Vector2f{cursor_pos.x, 0} - offset, {size / 10, 2 * size}}, {1, 1, 1, 1}
	);

	state.current_pos.y += cursor_pos.y + state.text_margin;

	return displayed;
}
