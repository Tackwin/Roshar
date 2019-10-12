#ifdef WEB

#include <OS/RealTimeIO.hpp>

Vector2f io::get_mouse_pos() noexcept {
	return {};
}

io::Keyboard_State io::get_keyboard_state() noexcept {
	return {};
}
io::Controller_State io::get_controller_state() noexcept {
	return {};
}
size_t io::map_key(size_t x) noexcept {
	return {};
}
size_t io::map_mouse(size_t x) noexcept {
	return {};
}
size_t io::map_controller(size_t x) noexcept {
	return {};
}

#endif