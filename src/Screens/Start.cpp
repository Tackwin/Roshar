#include "Screens/Start.hpp"

#include "Managers/AssetsManager.hpp"

void Start_Screen::input(IM::Input_Iterator it) noexcept {
	last_input = it;
}

void Start_Screen::update(float dt) noexcept {
	for (auto& [_, x] : button_states)
		if (x.hovering) x.time_since_hover += dt;
		else x.time_since_hover = 0;
}

void Start_Screen::render(render::Orders& target) noexcept {
	Vector2f s = { (float)Environment.window_width, (float)Environment.window_height };
	
	exit = button(target, { s.x / 2.f, 1 * s.y / 4.f }, "Quit", 42);
	goto_levels = button(target, { s.x / 2.f, 3 * s.y / 4.f }, "Play", 42);
	goto_settings = button(target, { s.x / 2.f, 2 * s.y / 4.f }, "Config", 42);
}

bool Start_Screen::button(
	render::Orders& target, Vector2f pos, std::string label, float size
) noexcept {
	target.late_push_view(
		{ 0.f, 0.f, (float)Environment.window_width, (float)Environment.window_height }
	);
	defer { target.late_pop_view(); };

	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);

	size += 0.25f * size * xstd::map_rp_to_01(10 * button_states[label].time_since_hover);

	auto rec = Rectanglef::centered(pos, 1.3f * font.compute_size(label, size));
	target.late_push_text(pos, asset::Font_Id::Consolas, label, size, { .5f, .5f });

	if (!IM::iterator_is_valid(last_input)) return false;

	bool hovering = rec.in(last_input->mouse_screen_pos);
	bool clicking = last_input->is_just_pressed(Mouse::Left);

	button_states[label].hovering = hovering;

	return hovering && clicking;
}
