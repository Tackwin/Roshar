#include "InputsManager.hpp"

#include "imgui.h"
#include "OS/file.hpp"
#include "OS/RealTimeIO.hpp"

[[nodiscard]] Vector2f Inputs_Info::mouse_world_pos(Rectanglef& v) const noexcept {
	render::View_Info info;
	info.screen_bounds = { {0, 0}, {1, 1} };
	info.world_bounds = v;
	return mouse_world_pos(info);
}
[[nodiscard]] Vector2f Inputs_Info::mouse_world_pos(render::View_Info& v) const noexcept {
	Rectangle_t<size_t> screen_space_bound = {
		{
			(size_t)(v.screen_bounds.x * window_size.x),
			(size_t)(v.screen_bounds.y * window_size.y)
		},
		{
			(size_t)(v.screen_bounds.w * window_size.x),
			(size_t)(v.screen_bounds.h * window_size.y)
		}
	};

	Vector2f normalized = {
		(mouse_screen_pos.x - screen_space_bound.x) / screen_space_bound.w,
		(mouse_screen_pos.y - screen_space_bound.y) / screen_space_bound.h
	};

	return {
		v.world_bounds.x + v.world_bounds.w * normalized.x,
		v.world_bounds.y + v.world_bounds.h * normalized.y
	};
}
[[nodiscard]] bool Inputs_Info::is_just_released(Keyboard::Key k) const noexcept {
	return key_infos[k].just_released;
}
[[nodiscard]] bool Inputs_Info::is_just_released(Mouse::Button b) const noexcept {
	return mouse_infos[b].just_released;
}
[[nodiscard]] bool Inputs_Info::is_just_released(Joystick::Button b) const noexcept {
	return joystick_buttons_infos[b].just_released;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(Keyboard::Key k) const noexcept {
	return key_infos[k].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(Mouse::Button b) const noexcept {
	return mouse_infos[b].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(Joystick::Button b) const noexcept {
	return joystick_buttons_infos[b].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(Keyboard::Key k) const noexcept {
	return key_infos[k].pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(Mouse::Button b) const noexcept {
	return mouse_infos[b].pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(Joystick::Button b) const noexcept {
	return joystick_buttons_infos[b].pressed;
}
[[nodiscard]] std::optional<Keyboard::Key> Inputs_Info::last_key_pressed() const noexcept {
	for (size_t i = 0; i < key_infos.size(); ++i) {
		if (key_infos[i].just_pressed) return (Keyboard::Key)i;
	}
	for (size_t i = 0; i < key_infos.size(); ++i) {
		if (key_infos[i].pressed) return (Keyboard::Key)i;
	}

	return std::nullopt;
}
[[nodiscard]] std::optional<Joystick::Button> Inputs_Info::last_contoller_pressed() const noexcept {
	for (size_t i = 0; i < joystick_buttons_infos.size(); ++i) {
		if (joystick_buttons_infos[i].just_pressed) return (Joystick::Button)i;
	}
	for (size_t i = 0; i < joystick_buttons_infos.size(); ++i) {
		if (joystick_buttons_infos[i].pressed) return (Joystick::Button)i;
	}

	return std::nullopt;
}


Keyboard::Key InputsManager::getLastKeyPressed() noexcept {
	for (auto it = records.rbegin(); it != records.rend(); ++it) {
		for (size_t i = 0; i < it->key_infos.size(); ++i)
			if (it->key_infos[i].just_pressed) return (Keyboard::Key)i;
	}

	return (Keyboard::Key)0;
}

bool InputsManager::isLastSequence(
const std::vector<Keyboard::Key>& keys,
const std::vector<Keyboard::Key>& modifiers
) noexcept {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	if (!last_record.key_captured) return false;
	if (keys.size() > records.size()) return false;

	auto it = records.rbegin();
	for (size_t i = keys.size() - 1; i + 1 > 0; --i) {
		const auto& current_key = keys[i];

		bool flag = false;
		for (; it != records.rend(); ++it) {
			for (size_t j = 0; j < it->key_infos.size(); ++j) {
				if (it->key_infos[j].just_pressed) {
					if (current_key != (Keyboard::Key)j) return false;
					flag = true;
					break;
				}
			}
			if (flag) break;
		}

		if (!flag) return false;
	}

	for (auto& x : modifiers) if (!isKeyPressed(x)) return false;

	return true;
}
bool InputsManager::isLastSequenceJustFinished(
	std::initializer_list<Keyboard::Key> keys,
	std::initializer_list<Keyboard::Key> modifiers
) noexcept {
	if (records.empty()) return false;

	const auto& last_record = records.back();
	return (!last_record.key_captured && isKeyJustPressed() && isLastSequence(keys, modifiers));
}

bool InputsManager::isKeyJustPressed() noexcept {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	for (const auto& x : last_record.key_infos) if (x.just_pressed) return true;
	return false;
}
bool InputsManager::isKeyJustReleased() noexcept {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	for (const auto& x : last_record.key_infos) if (x.just_released) return true;
	return false;
}

bool InputsManager::isKeyPressed() noexcept {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	for (const auto& x : last_record.key_infos) if (x.pressed) return true;
	return false;
}
bool InputsManager::isKeyPressed(const Keyboard::Key &key) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.key_captured && last_record.key_infos[key].pressed;
}
bool InputsManager::isKeyJustPressed(const Keyboard::Key &key) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.key_captured && last_record.key_infos[key].just_pressed;
}
bool IM::isKeyJustPressed(Joystick::Button key) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return last_record.joystick_buttons_infos[key].just_pressed;
}

bool InputsManager::isKeyJustReleased(const Keyboard::Key &key) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.key_captured && last_record.key_infos[key].just_released;
}

bool InputsManager::isMousePressed(const Mouse::Button &button) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.mouse_captured && last_record.mouse_infos[button].pressed;
}
bool InputsManager::isMouseJustPressed(const Mouse::Button &button) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.mouse_captured && last_record.mouse_infos[button].just_pressed;
}
bool InputsManager::isMouseJustReleased(const Mouse::Button &button) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.mouse_captured && last_record.mouse_infos[button].just_released;
}

Vector2f IM::getMousePosInView(const Rectanglef& view) noexcept {
	render::View_Info i;
	i.screen_bounds = { {0, 0}, {1, 1} };
	i.world_bounds = view;
	return getMousePosInView(i);
}
Vector2f IM::getMousePosInView(const render::View_Info& view) noexcept {
	return applyInverseView(view, getMouseScreenPos());
}

Vector2f InputsManager::getMouseDeltaInView(const Rectanglef& view) noexcept {
	render::View_Info info;
	info.screen_bounds = { {0, 0}, {1, 1} };
	info.world_bounds = view;
	return getMouseDeltaInView(info);
}
Vector2f InputsManager::getMouseDeltaInView(const render::View_Info& view) noexcept {
	auto A = getMouseScreenPos();
	auto B = A - getMouseScreenDelta();

	return applyInverseView(view, A) - applyInverseView(view, B);
}
Vector2f InputsManager::getMouseScreenPos() {
	if (records.empty()) return {};

	const auto& last_record = records.back();

	return last_record.mouse_screen_pos;
}
Vector2f InputsManager::getMouseScreenDelta() noexcept {
	if (records.empty()) return {};

	const auto& last_record = records.back();

	return last_record.mouse_screen_delta;
}
float InputsManager::getLastScroll() noexcept {
	if (records.empty()) return {};

	const auto& last_record = records.back();

	return !last_record.mouse_captured ? last_record.scroll : 0;
}

float IM::get_dt() noexcept {
	if (records.size() < 1) return 0;

	const auto& last_record = records.back();

	return last_record.dt;
}

auto IM::get_iterator() noexcept -> decltype(records)::iterator {
	return records.empty() ? records.end() : --records.end();
}

void InputsManager::update(float dt) {
	defer{ wheel_scroll = 0; };
	Inputs_Info new_record = {};

	new_record.dt = dt;

	auto keyboard_state = io::get_keyboard_state();
	for (size_t i = 0; i < io::Keyboard::Count; ++i) {
		auto vk = map_key((io::Keyboard::Key)i);
		auto pressed = (bool)keyboard_state.keys[vk];
		auto last_pressed = records.empty() ? false : records.back().key_infos[i].pressed;

		new_record.key_infos[i].just_pressed = !last_pressed && pressed;
		new_record.key_infos[i].just_released = last_pressed && !pressed;
		new_record.key_infos[i].pressed = pressed;
	}

	for (size_t i = 0; i < io::Mouse::Count; ++i) {
		auto pressed = (bool)keyboard_state.keys[map_mouse((io::Mouse::Button)i)];
		auto last_pressed = records.empty() ? false : records.back().mouse_infos[i].pressed;

		new_record.mouse_infos[i].just_pressed = !last_pressed && pressed;
		new_record.mouse_infos[i].just_released = last_pressed && !pressed;
		new_record.mouse_infos[i].pressed = pressed;
	}

	auto state = io::get_controller_state(controller_idx);
	auto Max_Range = .5f + (1 << (8 * sizeof(state.left_thumb_x) - 1));

	new_record.left_trigger = state.left_trigger / 255.f;
	new_record.right_trigger = state.right_trigger / 255.f;
	new_record.left_trigger *= new_record.left_trigger;
	new_record.right_trigger *= new_record.right_trigger;

	new_record.left_joystick = {
		(state.left_thumb_x + 0.5f) / Max_Range,
		(state.left_thumb_y + 0.5f) / Max_Range
	};
	new_record.right_joystick = {
		(state.right_thumb_x + 0.5f) / Max_Range,
		(state.right_thumb_y + 0.5f) / Max_Range
	};

	auto dead_range = (io::Controller_State::Left_Thumb_Deadzone + .5) / (double)Max_Range;
	auto length = new_record.left_joystick.length();
	if (length < dead_range) {
		new_record.left_joystick = {};
	}
	new_record.left_joystick.applyCW([](auto x) { return std::powf(x, 1); });

	length = new_record.right_joystick.length();
	if (length < dead_range) {
		new_record.right_joystick = {};
	}
	new_record.right_joystick.applyCW([](auto x) { return std::powf(x, 1); });

	for (size_t i = 0; i < io::Controller::Count; ++i) {
		auto pressed = (bool)(state.buttons_mask & io::map_controller((io::Controller::Button)i));
		auto last_pressed =
			records.empty() ? false : records.back().joystick_buttons_infos[i].pressed;
		new_record.joystick_buttons_infos[i].just_pressed = !last_pressed && pressed;
		new_record.joystick_buttons_infos[i].just_released = last_pressed && !pressed;
		new_record.joystick_buttons_infos[i].pressed = pressed;
	}
	
	new_record.focused = true;
	new_record.scroll = wheel_scroll;

	Vector2f last_mouse_screen_pos =
		records.empty() ? Vector2f{} : records.back().mouse_screen_pos;

	new_record.mouse_screen_pos = io::get_mouse_pos();
	new_record.mouse_screen_delta = new_record.mouse_screen_pos - last_mouse_screen_pos;

	new_record.window_size = { Environment.window_width, Environment.window_height };

	new_record.key_captured = ImGui::GetIO().WantCaptureKeyboard;
	new_record.mouse_captured = ImGui::GetIO().WantCaptureMouse;

	records.push_back(new_record);
}

Vector2f IM::applyInverseView(const Rectanglef& view, Vector2f p) noexcept {
	render::View_Info info;
	info.screen_bounds = { {0, 0}, {1, 1} };
	info.world_bounds = view;
	return applyInverseView(info, p);
}

Vector2f IM::applyInverseView(const render::View_Info& view, Vector2f p) noexcept {
	if (records.empty()) return {};

	const auto& last_record = records.back();

	Rectangle_t<size_t> screen_space_bound = {
		{
			(size_t)(view.screen_bounds.x * last_record.window_size.x),
			(size_t)(view.screen_bounds.y * last_record.window_size.y)
		},
		{
			(size_t)(view.screen_bounds.w * last_record.window_size.x),
			(size_t)(view.screen_bounds.h * last_record.window_size.y)
		}
	};

	Vector2f normalized = {
		(p.x - screen_space_bound.x) / screen_space_bound.w,
		(p.y - screen_space_bound.y) / screen_space_bound.h
	};

	return {
		view.world_bounds.x + view.world_bounds.w * normalized.x,
		view.world_bounds.y + view.world_bounds.h * normalized.y
	};
}

bool IM::isWindowFocused() noexcept {
	if (records.empty()) return {};

	const auto& last_record = records.back();

	return last_record.focused;
}

Vector2u IM::getWindowSize() noexcept {
	if (records.empty()) return {};

	const auto& last_record = records.back();

	return last_record.window_size;
}


// We don't care about cross machine permanence so our work is greatly reduced
bool IM::save_range(
std::filesystem::path path, Input_Iterator begin, Input_Iterator end
) noexcept {
	std::vector<std::uint8_t> bytes;
	bytes.push_back(Inputs_Info::Version);

	for (auto it = begin; it != end; ++it) {
		auto buffer = reinterpret_cast<const std::uint8_t*>(&(*it));
		for (size_t i = 0; i < sizeof(Inputs_Info); ++i) {
			bytes.push_back(buffer[i]);
		}
	}

	return file::overwrite_file_byte(path, bytes) == file::Error::No_Error;
}

std::uint64_t IM::load_record(std::filesystem::path path) noexcept {
	auto id = xstd::uuid();

	return load_record_at(path, id) ? id : 0;
}


struct Inputs_Info_1 {
	constexpr static float Joystick_Dead_Zone{ 10 };
	constexpr static std::uint8_t Version{ 1 };

	struct Action_Info {
		bool pressed : 1;
		bool just_pressed : 1;
		bool just_released : 1;
	};

	std::array<Action_Info, Keyboard::Count>        key_infos;
	std::array<Action_Info, Mouse::Count>           mouse_infos;
	std::array<Action_Info, 32> joystick_buttons_infos;

	Vector2f joystick_axis;

	Vector2f mouse_screen_pos;
	Vector2f mouse_screen_delta;
	Vector2u window_size;

	float scroll;
	float dt;

	struct {
		bool mouse_captured : 1;
		bool key_captured : 1;
		bool focused : 1;
	};
};

Inputs_Info update(Inputs_Info_1 old) noexcept {
	Inputs_Info new_info = {};
	new_info.dt = old.dt;
	new_info.focused = old.focused;
	for (size_t i = 0; i < old.joystick_buttons_infos.size(); ++i) {
		auto& n = new_info.joystick_buttons_infos[i];
		const auto& o = old.joystick_buttons_infos[i];

		n.just_pressed = o.just_pressed;
		n.just_released = o.just_released;
		n.pressed = o.pressed;
	}
	new_info.key_captured = old.key_captured;
	new_info.mouse_captured = old.mouse_captured;
	for (size_t i = 0; i < old.key_infos.size(); ++i) {
		auto& n = new_info.key_infos[i];
		const auto& o = old.key_infos[i];

		n.just_pressed = o.just_pressed;
		n.just_released = o.just_released;
		n.pressed = o.pressed;
	}

	for (size_t i = 0; i < old.mouse_infos.size(); ++i) {
		auto& n = new_info.mouse_infos[i];
		const auto& o = old.mouse_infos[i];

		n.just_pressed = o.just_pressed;
		n.just_released = o.just_released;
		n.pressed = o.pressed;
	}

	new_info.mouse_screen_delta = old.mouse_screen_delta;
	new_info.mouse_screen_pos = old.mouse_screen_pos;
	new_info.scroll = old.scroll;
	new_info.window_size = old.window_size;
	return new_info;
}

bool IM::load_record_at(std::filesystem::path path, std::uint64_t id) noexcept {
	auto expected = file::read_whole_file(path);
	if (!expected) return 0;
	loaded_record[id].clear();
	auto bytes = *expected;

	std::uint8_t version = bytes[0];
	switch (version) {
	case Inputs_Info_1::Version: {
		for (size_t i = 1; i < bytes.size();) {
			Inputs_Info_1 info;
			for (size_t j = 0; j < sizeof(info); ++j, ++i) {
				reinterpret_cast<std::uint8_t*>(&info)[j] = bytes[i];
			}
			loaded_record[id].push_back(::update(info));
		}
		return true;
	}
	case Inputs_Info::Version: {
		for (size_t i = 1; i < bytes.size();) {
			Inputs_Info info;
			for (size_t j = 0; j < sizeof(Inputs_Info); ++j, ++i) {
				reinterpret_cast<std::uint8_t*>(&info)[j] = bytes[i];
			}
			loaded_record[id].push_back(info);
		}
		return true;
	}
	default: assert("Logic error");
	}
	return false;
}

Input_Iterator IM::begin(std::uint64_t id) noexcept {
	return std::begin(loaded_record[id]);
}
Input_Iterator IM::end(std::uint64_t id) noexcept {
	return std::end(loaded_record[id]);
}

size_t IM::size(std::uint64_t id) noexcept {
	return std::size(loaded_record[id]);
}

void IM::forget_record(std::uint64_t id) noexcept {
	if (loaded_record.count(id)) loaded_record.erase(id);
}

bool IM::iterator_is_valid() noexcept{
	return get_iterator() != std::end(records);
}

bool IM::iterator_is_valid(IM::Input_Iterator it) noexcept{
	return it != std::end(records);
}
IM::Input_Iterator IM::end() noexcept{
	return std::end(records);
}

std::string to_string(Keyboard::Key k) noexcept {
	switch (k) {
		case Keyboard::Key::A : {
			return "A";
		}
		case Keyboard::Key::B : {
			return "B";
		}
		case Keyboard::Key::C : {
			return "C";
		}
		case Keyboard::Key::D : {
			return "D";
		}
		case Keyboard::Key::E : {
			return "E";
		}
		case Keyboard::Key::F : {
			return "F";
		}
		case Keyboard::Key::G : {
			return "G";
		}
		case Keyboard::Key::H : {
			return "H";
		}
		case Keyboard::Key::I : {
			return "I";
		}
		case Keyboard::Key::J : {
			return "J";
		}
		case Keyboard::Key::K : {
			return "K";
		}
		case Keyboard::Key::L : {
			return "L";
		}
		case Keyboard::Key::M : {
			return "M";
		}
		case Keyboard::Key::N : {
			return "N";
		}
		case Keyboard::Key::O : {
			return "O";
		}
		case Keyboard::Key::P : {
			return "P";
		}
		case Keyboard::Key::Q : {
			return "Q";
		}
		case Keyboard::Key::R : {
			return "R";
		}
		case Keyboard::Key::S : {
			return "S";
		}
		case Keyboard::Key::T : {
			return "T";
		}
		case Keyboard::Key::U : {
			return "U";
		}
		case Keyboard::Key::V : {
			return "V";
		}
		case Keyboard::Key::W : {
			return "W";
		}
		case Keyboard::Key::X : {
			return "X";
		}
		case Keyboard::Key::Y : {
			return "Y";
		}
		case Keyboard::Key::Z : {
			return "Z";
		}
		case Keyboard::Key::Num0 : {
			return "Num0";
		}
		case Keyboard::Key::Num1 : {
			return "Num1";
		}
		case Keyboard::Key::Num2 : {
			return "Num2";
		}
		case Keyboard::Key::Num3 : {
			return "Num3";
		}
		case Keyboard::Key::Num4 : {
			return "Num4";
		}
		case Keyboard::Key::Num5 : {
			return "Num5";
		}
		case Keyboard::Key::Num6 : {
			return "Num6";
		}
		case Keyboard::Key::Num7 : {
			return "Num7";
		}
		case Keyboard::Key::Num8 : {
			return "Num8";
		}
		case Keyboard::Key::Num9 : {
			return "Num9";
		}
		case Keyboard::Key::Escape : {
			return "Escape";
		}
		case Keyboard::Key::LCTRL : {
			return "LCTRL";
		}
		case Keyboard::Key::LSHIFT : {
			return "LSHIFT";
		}
		case Keyboard::Key::LALT : {
			return "LALT";
		}
		case Keyboard::Key::LSYS : {
			return "LSYS";
		}
		case Keyboard::Key::RCTRL : {
			return "RCTRL";
		}
		case Keyboard::Key::RSHIFT : {
			return "RSHIFT";
		}
		case Keyboard::Key::RALT : {
			return "RALT";
		}
		case Keyboard::Key::RSYS : {
			return "RSYS";
		}
		case Keyboard::Key::Menu : {
			return "Menu";
		}
		case Keyboard::Key::LBracket : {
			return "LBracket";
		}
		case Keyboard::Key::RBracket : {
			return "RBracket";
		}
		case Keyboard::Key::Semicolon : {
			return "Semicolon";
		}
		case Keyboard::Key::Comma : {
			return "Comma";
		}
		case Keyboard::Key::Period : {
			return "Period";
		}
		case Keyboard::Key::Quote : {
			return "Quote";
		}
		case Keyboard::Key::Slash : {
			return "Slash";
		}
		case Keyboard::Key::Backslash : {
			return "Backslash";
		}
		case Keyboard::Key::Tilde : {
			return "Tilde";
		}
		case Keyboard::Key::Equal : {
			return "Equal";
		}
		case Keyboard::Key::Hyphen : {
			return "Hyphen";
		}
		case Keyboard::Key::Space : {
			return "Space";
		}
		case Keyboard::Key::Return : {
			return "Return";
		}
		case Keyboard::Key::Backspace : {
			return "Backspace";
		}
		case Keyboard::Key::Tab : {
			return "Tab";
		}
		case Keyboard::Key::PageUp : {
			return "PageUp";
		}
		case Keyboard::Key::PageDown : {
			return "PageDown";
		}
		case Keyboard::Key::End : {
			return "End";
		}
		case Keyboard::Key::Home : {
			return "Home";
		}
		case Keyboard::Key::Insert : {
			return "Insert";
		}
		case Keyboard::Key::DEL : {
			return "DEL";
		}
		case Keyboard::Key::Add : {
			return "Add";
		}
		case Keyboard::Key::Subtract : {
			return "Subtract";
		}
		case Keyboard::Key::Multiply : {
			return "Multiply";
		}
		case Keyboard::Key::Divide : {
			return "Divide";
		}
		case Keyboard::Key::Left : {
			return "Left";
		}
		case Keyboard::Key::Right : {
			return "Right";
		}
		case Keyboard::Key::Up : {
			return "Up";
		}
		case Keyboard::Key::Down : {
			return "Down";
		}
		case Keyboard::Key::Numpad0 : {
			return "Numpad0";
		}
		case Keyboard::Key::Numpad1 : {
			return "Numpad1";
		}
		case Keyboard::Key::Numpad2 : {
			return "Numpad2";
		}
		case Keyboard::Key::Numpad3 : {
			return "Numpad3";
		}
		case Keyboard::Key::Numpad4 : {
			return "Numpad4";
		}
		case Keyboard::Key::Numpad5 : {
			return "Numpad5";
		}
		case Keyboard::Key::Numpad6 : {
			return "Numpad6";
		}
		case Keyboard::Key::Numpad7 : {
			return "Numpad7";
		}
		case Keyboard::Key::Numpad8 : {
			return "Numpad8";
		}
		case Keyboard::Key::Numpad9 : {
			return "Numpad9";
		}
		case Keyboard::Key::F1 : {
			return "F1";
		}
		case Keyboard::Key::F2 : {
			return "F2";
		}
		case Keyboard::Key::F3 : {
			return "F3";
		}
		case Keyboard::Key::F4 : {
			return "F4";
		}
		case Keyboard::Key::F5 : {
			return "F5";
		}
		case Keyboard::Key::F6 : {
			return "F6";
		}
		case Keyboard::Key::F7 : {
			return "F7";
		}
		case Keyboard::Key::F8 : {
			return "F8";
		}
		case Keyboard::Key::F9 : {
			return "F9";
		}
		case Keyboard::Key::F10 : {
			return "F10";
		}
		case Keyboard::Key::F11 : {
			return "F11";
		}
		case Keyboard::Key::F12 : {
			return "F12";
		}
		case Keyboard::Key::F13 : {
			return "F13";
		}
		case Keyboard::Key::F14 : {
			return "F14";
		}
		case Keyboard::Key::F15 : {
			return "F15";
		}
		case Keyboard::Key::Pause : {
			return "Pause";
		}
		default: {
			return "Unknown";
		}
	}
}

std::string to_string(Joystick::Button b) noexcept {
	switch (b) {
		case Joystick::Button::A : {
			return "A";
		}
		case Joystick::Button::B : {
			return "B";
		}
		case Joystick::Button::X : {
			return "X";
		}
		case Joystick::Button::Y : {
			return "Y";
		}
		case Joystick::Button::DPAD_UP : {
			return "DPAD_UP";
		}
		case Joystick::Button::DPAD_DOWN : {
			return "DPAD_DOWN";
		}
		case Joystick::Button::DPAD_LEFT : {
			return "DPAD_LEFT";
		}
		case Joystick::Button::DPAD_RIGHT : {
			return "DPAD_RIGHT";
		}
		case Joystick::Button::START : {
			return "START";
		}
		case Joystick::Button::BACK : {
			return "BACK";
		}
		case Joystick::Button::LB : {
			return "LB";
		}
		case Joystick::Button::RB : {
			return "RB";
		}
		case Joystick::Button::LT : {
			return "LT";
		}
		case Joystick::Button::RT : {
			return "RT";
		}
		default:
			return "Unknown";
			break;
	}
}
