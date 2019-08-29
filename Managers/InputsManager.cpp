#include "InputsManager.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <cassert>
#include <Windows.h>

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "../OS/file.hpp"

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
[[nodiscard]] bool Inputs_Info::is_just_released(int x              ) const noexcept {
	return joystick_buttons_infos[x].just_released;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(Keyboard::Key k) const noexcept {
	return key_infos[k].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(Mouse::Button b) const noexcept {
	return mouse_infos[b].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(int x              ) const noexcept {
	return joystick_buttons_infos[x].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(Keyboard::Key k) const noexcept {
	return key_infos[k].pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(Mouse::Button b) const noexcept {
	return mouse_infos[b].pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(int x              ) const noexcept {
	return joystick_buttons_infos[x].pressed;
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

	BYTE keyboard_state[256] = {};
	GetKeyboardState(keyboard_state);
	for (size_t i = 0; i < Keyboard::Count; ++i) {
		auto vk = get_vkey((Keyboard::Key)i);
		auto pressed = (bool)(keyboard_state[vk] & 0b10000000);
		auto last_pressed = records.empty() ? false : records.back().key_infos[i].pressed;

		new_record.key_infos[i].just_pressed = !last_pressed && pressed;
		new_record.key_infos[i].just_released = last_pressed && !pressed;
		new_record.key_infos[i].pressed = pressed;
	}

	for (size_t i = 0; i < Mouse::Count; ++i) {
		auto pressed = (bool)(keyboard_state[get_vkey((Mouse::Button)i)] & 0b10000000);
		auto last_pressed = records.empty() ? false : records.back().mouse_infos[i].pressed;

		new_record.mouse_infos[i].just_pressed = !last_pressed && pressed;
		new_record.mouse_infos[i].just_released = last_pressed && !pressed;
		new_record.mouse_infos[i].pressed = pressed;
	}
	/*
	for (size_t i = 0; i < sf::Joystick::ButtonCount; ++i) {
		auto pressed = sf::Joystick::isButtonPressed(0, i);
        
		auto last_pressed =
			records.empty() ? false : records.back().joystick_buttons_infos[i].pressed;
        
		new_record.joystick_buttons_infos[i].just_pressed = !last_pressed && pressed;
		new_record.joystick_buttons_infos[i].just_released = last_pressed && !pressed;
		new_record.joystick_buttons_infos[i].pressed = pressed;
	}
    
	new_record.joystick_axis = {
		sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X),
		-sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y)
	};*/

	new_record.scroll = wheel_scroll;
	new_record.focused = GetForegroundWindow() == (HWND)platform::handle_window;
	Vector2f last_mouse_screen_pos =
		records.empty() ? Vector2f{} : records.back().mouse_screen_pos;

	POINT p;
	GetCursorPos(&p);
	ScreenToClient((HWND)platform::handle_window, &p);

	new_record.mouse_screen_pos = {
		(float)p.x,
		(float)(Environment.window_height - p.y)
	};
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
	bytes.push_back(Inputs_Info::Version); // Version

	for (auto it = begin; it != end; ++it) {
		auto buffer = reinterpret_cast<const std::uint8_t*>(&(*it));
		for (size_t i = 0; i < sizeof(Inputs_Info); ++i) {
			bytes.push_back(buffer[i]);
		}
	}

	return file::overwrite_file_byte(path, bytes) == Error::No_Error;
}

std::uint64_t IM::load_record(std::filesystem::path path) noexcept {
	auto id = xstd::uuid();

	return load_record_at(path, id) ? id : 0;
}

bool IM::load_record_at(std::filesystem::path path, std::uint64_t id) noexcept {
	auto expected = file::read_whole_file(path);
	if (!expected) return 0;
	loaded_record[id].clear();
	auto bytes = *expected;

	std::uint8_t version = bytes[0];
	switch (version) {
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

int IM::get_vkey(Keyboard::Key key) noexcept {
	switch (key)
	{
		default:                       return 0;            
		case Keyboard::A:          return 'A';          
		case Keyboard::B:          return 'B';          
		case Keyboard::C:          return 'C';          
		case Keyboard::D:          return 'D';          
		case Keyboard::E:          return 'E';          
		case Keyboard::F:          return 'F';          
		case Keyboard::G:          return 'G';          
		case Keyboard::H:          return 'H';          
		case Keyboard::I:          return 'I';          
		case Keyboard::J:          return 'J';          
		case Keyboard::K:          return 'K';          
		case Keyboard::L:          return 'L';          
		case Keyboard::M:          return 'M';          
		case Keyboard::N:          return 'N';          
		case Keyboard::O:          return 'O';          
		case Keyboard::P:          return 'P';          
		case Keyboard::Q:          return 'Q';          
		case Keyboard::R:          return 'R';          
		case Keyboard::S:          return 'S';          
		case Keyboard::T:          return 'T';          
		case Keyboard::U:          return 'U';          
		case Keyboard::V:          return 'V';          
		case Keyboard::W:          return 'W';          
		case Keyboard::X:          return 'X';          
		case Keyboard::Y:          return 'Y';          
		case Keyboard::Z:          return 'Z';          
		case Keyboard::Escape:     return VK_ESCAPE;    
		case Keyboard::LCTRL:      return VK_LCONTROL;  
		case Keyboard::LSHIFT:     return VK_LSHIFT;    
		case Keyboard::LALT:       return VK_LMENU;     
		case Keyboard::LSYS:       return VK_LWIN;      
		case Keyboard::RCTRL:      return VK_RCONTROL;  
		case Keyboard::RSHIFT:     return VK_RSHIFT;    
		case Keyboard::RALT:       return VK_RMENU;     
		case Keyboard::RSYS:       return VK_RWIN;      
		case Keyboard::Quote:      return VK_OEM_7;     
		case Keyboard::F1:         return VK_F1;        
		case Keyboard::F2:         return VK_F2;        
		case Keyboard::F3:         return VK_F3;        
		case Keyboard::F4:         return VK_F4;        
		case Keyboard::F5:         return VK_F5;        
		case Keyboard::F6:         return VK_F6;        
		case Keyboard::F7:         return VK_F7;        
		case Keyboard::F8:         return VK_F8;        
		case Keyboard::F9:         return VK_F9;        
		case Keyboard::F10:        return VK_F10;       
		case Keyboard::F11:        return VK_F11;       
		case Keyboard::F12:        return VK_F12;       
		case Keyboard::Space:      return VK_SPACE;     
		case Keyboard::DEL:        return VK_DELETE;    
		case Keyboard::Return:     return VK_RETURN;
		/*case Keyboard::Num0:       return '0';          
		case Keyboard::Num1:       return '1';          
		case Keyboard::Num2:       return '2';          
		case Keyboard::Num3:       return '3';          
		case Keyboard::Num4:       return '4';          
		case Keyboard::Num5:       return '5';          
		case Keyboard::Num6:       return '6';          
		case Keyboard::Num7:       return '7';          
		case Keyboard::Num8:       return '8';          
		case Keyboard::Num9:       return '9';          
		case Keyboard::Menu:       return VK_APPS;      
		case Keyboard::LBracket:   return VK_OEM_4;     
		case Keyboard::RBracket:   return VK_OEM_6;     
		case Keyboard::Semicolon:  return VK_OEM_1;     
		case Keyboard::Comma:      return VK_OEM_COMMA; 
		case Keyboard::Period:     return VK_OEM_PERIOD;
		case Keyboard::Slash:      return VK_OEM_2;     
		case Keyboard::Backslash:  return VK_OEM_5;     
		case Keyboard::Tilde:      return VK_OEM_3;     
		case Keyboard::Equal:      return VK_OEM_PLUS;  
		case Keyboard::Hyphen:     return VK_OEM_MINUS; 
		case Keyboard::Enter:      return VK_RETURN;    
		case Keyboard::Backspace:  return VK_BACK;      
		case Keyboard::Tab:        return VK_TAB;       
		case Keyboard::PageUp:     return VK_PRIOR;     
		case Keyboard::PageDown:   return VK_NEXT;      
		case Keyboard::End:        return VK_END;       
		case Keyboard::Home:       return VK_HOME;      
		case Keyboard::Insert:     return VK_INSERT;    
		case Keyboard::Add:        return VK_ADD;       
		case Keyboard::Subtract:   return VK_SUBTRACT;  
		case Keyboard::Multiply:   return VK_MULTIPLY;  
		case Keyboard::Divide:     return VK_DIVIDE;    
		case Keyboard::Left:       return VK_LEFT;      
		case Keyboard::Right:      return VK_RIGHT;     
		case Keyboard::Up:         return VK_UP;        
		case Keyboard::Down:       return VK_DOWN;      
		case Keyboard::Numpad0:    return VK_NUMPAD0;   
		case Keyboard::Numpad1:    return VK_NUMPAD1;   
		case Keyboard::Numpad2:    return VK_NUMPAD2;   
		case Keyboard::Numpad3:    return VK_NUMPAD3;   
		case Keyboard::Numpad4:    return VK_NUMPAD4;   
		case Keyboard::Numpad5:    return VK_NUMPAD5;   
		case Keyboard::Numpad6:    return VK_NUMPAD6;   
		case Keyboard::Numpad7:    return VK_NUMPAD7;   
		case Keyboard::Numpad8:    return VK_NUMPAD8;   
		case Keyboard::Numpad9:    return VK_NUMPAD9;   
		case Keyboard::F13:        return VK_F13;       
		case Keyboard::F14:        return VK_F14;       
		case Keyboard::F15:        return VK_F15;       
		case Keyboard::Pause:      return VK_PAUSE;     */
	}
	assert("Logic error.");
	return 0;
}

int IM::get_vkey(Mouse::Button key) noexcept {
	switch (key) {
	case Mouse::Button::Left:   return VK_LBUTTON;
	case Mouse::Button::Right:  return VK_RBUTTON;
	case Mouse::Button::Middle: return VK_MBUTTON;
	}
	assert("Logic error.");
	return 0;
}

