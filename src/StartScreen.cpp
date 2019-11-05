#include "StartScreen.hpp"

#include "Managers/AssetsManager.hpp"

void Start_Screen::input(IM::Input_Iterator it) noexcept {
	last_input = it;
}

void Start_Screen::update(std::uint64_t dt) noexcept {

}

void Start_Screen::render(render::Orders& target) noexcept {
	Vector2f s = { (float)Environment.window_width, (float)Environment.window_height };
	
	exit = button(target, { s.x / 2.f, 1 * s.y / 4.f }, "Quit", 42);
	goto_levels = button(target, { s.x / 2.f, 3 * s.y / 4.f }, "Play", 42);
	goto_settings = button(target, { s.x / 2.f, 2 * s.y / 4.f }, "Setttings", 42);
}

bool Start_Screen::button(
	render::Orders& target, Vector2f pos, std::string label, float size
) noexcept {
	target.late_push_view(
		{ 0.f, 0.f, (float)Environment.window_width, (float)Environment.window_height }
	);
	defer { target.late_pop_view(); };

	auto& font = asset::Store.get_font(asset::Font_Id::Consolas);

	auto rec = Rectanglef{ pos, font.compute_size(label, size) };

	target.late_push_rec(rec, { 0, 0, 0, 1 });
	target.late_push_text(pos, asset::Font_Id::Consolas, label, size, { .5f, .5f });

	if (!IM::iterator_is_valid()) return false;

	return rec.in(last_input->mouse_screen_pos) && last_input->is_just_pressed(Mouse::Left);
}
