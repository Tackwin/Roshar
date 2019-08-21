#include "InputsManager.hpp"

#include <cassert>
#include <Windows.h>

#include "imgui.h"
#include "imgui-SFML.h"
#include "../OS/file.hpp"

[[nodiscard]] Vector2f Inputs_Info::mouse_world_pos(sf::View& v) const noexcept {
	const auto& viewScope = v.getViewport();

	auto viewPort = sf::IntRect(
		(int)std::ceil(window_size.x * viewScope.left),
		(int)std::ceil(window_size.y * viewScope.top),
		(int)std::ceil(window_size.x * viewScope.width),
		(int)std::ceil(window_size.y * viewScope.height)
	);

	Vector2f normalized;

	normalized.x = -1.f + 2.f * (mouse_screen_pos.x - viewPort.left) / viewPort.width;
	normalized.y = +1.f - 2.f * (mouse_screen_pos.y - viewPort.top) / viewPort.height;

	return v.getInverseTransform().transformPoint(normalized);
}
[[nodiscard]] bool Inputs_Info::is_just_released(sf::Keyboard::Key k) const noexcept {
	return key_infos[k].just_released;
}
[[nodiscard]] bool Inputs_Info::is_just_released(sf::Mouse::Button b) const noexcept {
	return mouse_infos[b].just_released;
}
[[nodiscard]] bool Inputs_Info::is_just_released(int x              ) const noexcept {
	return joystick_buttons_infos[x].just_released;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(sf::Keyboard::Key k) const noexcept {
	return key_infos[k].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(sf::Mouse::Button b) const noexcept {
	return mouse_infos[b].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(int x              ) const noexcept {
	return joystick_buttons_infos[x].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(sf::Keyboard::Key k) const noexcept {
	return key_infos[k].pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(sf::Mouse::Button b) const noexcept {
	return mouse_infos[b].pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(int x              ) const noexcept {
	return joystick_buttons_infos[x].pressed;
}

void print_sequence(const std::vector<sf::Keyboard::Key>& x) {
	for (auto& k : x) {
		printf("%s ", IM::nameOfKey(k).c_str());
	}
	printf("\n");
}

InputsManager::InputsManager() {}
InputsManager::~InputsManager() {}

std::string InputsManager::nameOfKey(sf::Keyboard::Key k) noexcept {
#define X(a) case sf::Keyboard::a: return #a;
	switch (k)
	{
	X(Unknown)
	X(A)
	X(B)
	X(C)
	X(D)
	X(E)
	X(F)
	X(G)
	X(H)
	X(I)
	X(J)
	X(K)
	X(L)
	X(M)
	X(N)
	X(O)
	X(P)
	X(Q)
	X(R)
	X(S)
	X(T)
	X(U)
	X(V)
	X(W)
	X(X)
	X(Y)
	X(Z)
	X(Num0)
	X(Num1)
	X(Num2)
	X(Num3)
	X(Num4)
	X(Num5)
	X(Num6)
	X(Num7)
	X(Num8)
	X(Num9)
	X(Escape)
	X(LControl)
	X(LShift)
	X(LAlt)
	X(LSystem)
	X(RControl)
	X(RShift)
	X(RAlt)
	X(RSystem)
	X(Menu)
	X(LBracket)
	X(RBracket)
	X(SemiColon)
	X(Comma)
	X(Period)
	X(Quote)
	X(Slash)
	X(BackSlash)
	X(Tilde)
	X(Equal)
	X(Dash)
	X(Space)
	X(Return)
	X(BackSpace)
	X(Tab)
	X(PageUp)
	X(PageDown)
	X(End)
	X(Home)
	X(Insert)
	X(Delete)
	X(Add)
	X(Subtract)
	X(Multiply)
	X(Divide)
	X(Left)
	X(Right)
	X(Up)
	X(Down)
	X(Numpad0)
	X(Numpad1)
	X(Numpad2)
	X(Numpad3)
	X(Numpad4)
	X(Numpad5)
	X(Numpad6)
	X(Numpad7)
	X(Numpad8)
	X(Numpad9)
	X(F1)
	X(F2)
	X(F3)
	X(F4)
	X(F5)
	X(F6)
	X(F7)
	X(F8)
	X(F9)
	X(F10)
	X(F11)
	X(F12)
	X(F13)
	X(F14)
	X(F15)
	X(Pause)
	X(KeyCount)
	}
	return assert("should not happen !"), "";
#undef X
}

sf::Keyboard::Key InputsManager::getLastKeyPressed() noexcept {
	for (auto it = records.rbegin(); it != records.rend(); ++it) {
		for (size_t i = 0; i < it->key_infos.size(); ++i)
			if (it->key_infos[i].just_pressed) return (sf::Keyboard::Key)i;
	}

	return (sf::Keyboard::Key)0;
}

bool InputsManager::isLastSequence(
	const std::vector<sf::Keyboard::Key>& keys,
	const std::vector<sf::Keyboard::Key>& modifiers
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
					if (current_key != (sf::Keyboard::Key)j) return false;
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
	std::initializer_list<sf::Keyboard::Key> keys,
	std::initializer_list<sf::Keyboard::Key> modifiers
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
bool InputsManager::isKeyPressed(const sf::Keyboard::Key &key) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.key_captured && last_record.key_infos[key].pressed;
}
bool InputsManager::isKeyJustPressed(const sf::Keyboard::Key &key) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.key_captured && last_record.key_infos[key].just_pressed;
}
bool InputsManager::isKeyJustReleased(const sf::Keyboard::Key &key) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.key_captured && last_record.key_infos[key].just_released;
}

bool InputsManager::isMousePressed(const sf::Mouse::Button &button) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.mouse_captured && last_record.mouse_infos[button].pressed;
}
bool InputsManager::isMouseJustPressed(const sf::Mouse::Button &button) {

	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.mouse_captured && last_record.mouse_infos[button].just_pressed;
}
bool InputsManager::isMouseJustReleased(const sf::Mouse::Button &button) {
	if (records.empty()) return false;

	const auto& last_record = records.back();

	return !last_record.mouse_captured && last_record.mouse_infos[button].just_released;
}

Vector2f InputsManager::getMousePosInView(const sf::View& view) {
	return applyInverseView(view, getMouseScreenPos());
}
Vector2f InputsManager::getMouseDeltaInView(const sf::View& view) noexcept {
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

	for (size_t i = 0; i < sf::Keyboard::KeyCount; ++i) {
		auto pressed = (bool)(keyboard_state[get_vkey((sf::Keyboard::Key)i)] & 0b1000'0000);
		auto last_pressed = records.empty() ? false : records.back().key_infos[i].pressed;

		new_record.key_infos[i].just_pressed = !last_pressed && pressed;
		new_record.key_infos[i].just_released = last_pressed && !pressed;
		new_record.key_infos[i].pressed = pressed;
	}

	for (size_t i = 0; i < sf::Mouse::ButtonCount; ++i) {
		auto pressed = sf::Mouse::isButtonPressed((sf::Mouse::Button)i);
		auto last_pressed = records.empty() ? false : records.back().mouse_infos[i].pressed;

		new_record.mouse_infos[i].just_pressed = !last_pressed && pressed;
		new_record.mouse_infos[i].just_released = last_pressed && !pressed;
		new_record.mouse_infos[i].pressed = pressed;
	}

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
	};

	new_record.scroll = wheel_scroll;
	new_record.focused = window->hasFocus();
	Vector2f last_mouse_screen_pos =
		records.empty() ? Vector2f{} : records.back().mouse_screen_pos;

	new_record.mouse_screen_pos = sf::Mouse::getPosition(*window);
	new_record.mouse_screen_delta = new_record.mouse_screen_pos - last_mouse_screen_pos;

	new_record.window_size = window->getSize();

	new_record.key_captured = ImGui::GetIO().WantCaptureKeyboard;
	new_record.mouse_captured = ImGui::GetIO().WantCaptureMouse;

	records.push_back(new_record);
}


Vector2f InputsManager::applyInverseView(const sf::View& view, Vector2f p) noexcept {
	const auto& viewScope = view.getViewport();
	const auto& window_size = getWindowSize();

	auto viewPort = sf::IntRect(
		(int)std::ceil(window_size.x * viewScope.left),
		(int)std::ceil(window_size.y * viewScope.top),
		(int)std::ceil(window_size.x * viewScope.width),
		(int)std::ceil(window_size.y * viewScope.height)
	);

	Vector2f normalized;

	normalized.x = -1.f + 2.f * (p.x - viewPort.left) / viewPort.width;
	normalized.y = +1.f - 2.f * (p.y - viewPort.top) / viewPort.height;

	return view.getInverseTransform().transformPoint(normalized);
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

struct Inputs_Info_0 {
	struct Action_Info {
		bool pressed : 1;
		bool just_pressed : 1;
		bool just_released : 1;
	};

	std::array<Action_Info, sf::Keyboard::KeyCount> key_infos;
	std::array<Action_Info, sf::Mouse::ButtonCount> mouse_infos;

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

Inputs_Info to_up_to_date(Inputs_Info_0 in) noexcept {
	Inputs_Info out = {};
	for (size_t i = 0; i < in.mouse_infos.size(); ++i) {
		out.mouse_infos[i].pressed       = in.mouse_infos[i].pressed;
		out.mouse_infos[i].just_pressed  = in.mouse_infos[i].just_pressed;
		out.mouse_infos[i].just_released = in.mouse_infos[i].just_released;
	}
	for (size_t i = 0; i < in.key_infos.size(); ++i) {
		out.key_infos[i].pressed       = in.key_infos[i].pressed;
		out.key_infos[i].just_pressed  = in.key_infos[i].just_pressed;
		out.key_infos[i].just_released = in.key_infos[i].just_released;
	}
	out.mouse_screen_pos = in.mouse_screen_pos;
	out.mouse_screen_delta = in.mouse_screen_delta;
	out.window_size = in.window_size;
	out.dt = in.dt;
	out.scroll = in.scroll;
	out.mouse_captured = in.mouse_captured;
	out.key_captured = in.key_captured;
	out.focused = in.focused;
	return out;
}

bool IM::load_record_at(std::filesystem::path path, std::uint64_t id) noexcept {
	auto expected = file::read_whole_file(path);
	if (!expected) return 0;
	loaded_record[id].clear();
	auto bytes = *expected;

	std::uint8_t version = bytes[0];
	switch (version) {
	case 0: {
		for (size_t i = 1; i < bytes.size();) {
			Inputs_Info_0 info;
			for (size_t j = 0; j < sizeof(Inputs_Info_0); ++j, ++i) {
				reinterpret_cast<std::uint8_t*>(&info)[j] = bytes[i];
			}
			loaded_record[id].push_back(to_up_to_date(info));
		}
		return true;
	}
	case 1: {
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

int IM::get_vkey(sf::Keyboard::Key key) noexcept {
	switch (key)
	{
	default:                       return 0;            
	case sf::Keyboard::A:          return 'A';          
	case sf::Keyboard::B:          return 'B';          
	case sf::Keyboard::C:          return 'C';          
	case sf::Keyboard::D:          return 'D';          
	case sf::Keyboard::E:          return 'E';          
	case sf::Keyboard::F:          return 'F';          
	case sf::Keyboard::G:          return 'G';          
	case sf::Keyboard::H:          return 'H';          
	case sf::Keyboard::I:          return 'I';          
	case sf::Keyboard::J:          return 'J';          
	case sf::Keyboard::K:          return 'K';          
	case sf::Keyboard::L:          return 'L';          
	case sf::Keyboard::M:          return 'M';          
	case sf::Keyboard::N:          return 'N';          
	case sf::Keyboard::O:          return 'O';          
	case sf::Keyboard::P:          return 'P';          
	case sf::Keyboard::Q:          return 'Q';          
	case sf::Keyboard::R:          return 'R';          
	case sf::Keyboard::S:          return 'S';          
	case sf::Keyboard::T:          return 'T';          
	case sf::Keyboard::U:          return 'U';          
	case sf::Keyboard::V:          return 'V';          
	case sf::Keyboard::W:          return 'W';          
	case sf::Keyboard::X:          return 'X';          
	case sf::Keyboard::Y:          return 'Y';          
	case sf::Keyboard::Z:          return 'Z';          
	case sf::Keyboard::Num0:       return '0';          
	case sf::Keyboard::Num1:       return '1';          
	case sf::Keyboard::Num2:       return '2';          
	case sf::Keyboard::Num3:       return '3';          
	case sf::Keyboard::Num4:       return '4';          
	case sf::Keyboard::Num5:       return '5';          
	case sf::Keyboard::Num6:       return '6';          
	case sf::Keyboard::Num7:       return '7';          
	case sf::Keyboard::Num8:       return '8';          
	case sf::Keyboard::Num9:       return '9';          
	case sf::Keyboard::Escape:     return VK_ESCAPE;    
	case sf::Keyboard::LControl:   return VK_LCONTROL;  
	case sf::Keyboard::LShift:     return VK_LSHIFT;    
	case sf::Keyboard::LAlt:       return VK_LMENU;     
	case sf::Keyboard::LSystem:    return VK_LWIN;      
	case sf::Keyboard::RControl:   return VK_RCONTROL;  
	case sf::Keyboard::RShift:     return VK_RSHIFT;    
	case sf::Keyboard::RAlt:       return VK_RMENU;     
	case sf::Keyboard::RSystem:    return VK_RWIN;      
	case sf::Keyboard::Menu:       return VK_APPS;      
	case sf::Keyboard::LBracket:   return VK_OEM_4;     
	case sf::Keyboard::RBracket:   return VK_OEM_6;     
	case sf::Keyboard::Semicolon:  return VK_OEM_1;     
	case sf::Keyboard::Comma:      return VK_OEM_COMMA; 
	case sf::Keyboard::Period:     return VK_OEM_PERIOD;
	case sf::Keyboard::Quote:      return VK_OEM_7;     
	case sf::Keyboard::Slash:      return VK_OEM_2;     
	case sf::Keyboard::Backslash:  return VK_OEM_5;     
	case sf::Keyboard::Tilde:      return VK_OEM_3;     
	case sf::Keyboard::Equal:      return VK_OEM_PLUS;  
	case sf::Keyboard::Hyphen:     return VK_OEM_MINUS; 
	case sf::Keyboard::Space:      return VK_SPACE;     
	case sf::Keyboard::Enter:      return VK_RETURN;    
	case sf::Keyboard::Backspace:  return VK_BACK;      
	case sf::Keyboard::Tab:        return VK_TAB;       
	case sf::Keyboard::PageUp:     return VK_PRIOR;     
	case sf::Keyboard::PageDown:   return VK_NEXT;      
	case sf::Keyboard::End:        return VK_END;       
	case sf::Keyboard::Home:       return VK_HOME;      
	case sf::Keyboard::Insert:     return VK_INSERT;    
	case sf::Keyboard::Delete:     return VK_DELETE;    
	case sf::Keyboard::Add:        return VK_ADD;       
	case sf::Keyboard::Subtract:   return VK_SUBTRACT;  
	case sf::Keyboard::Multiply:   return VK_MULTIPLY;  
	case sf::Keyboard::Divide:     return VK_DIVIDE;    
	case sf::Keyboard::Left:       return VK_LEFT;      
	case sf::Keyboard::Right:      return VK_RIGHT;     
	case sf::Keyboard::Up:         return VK_UP;        
	case sf::Keyboard::Down:       return VK_DOWN;      
	case sf::Keyboard::Numpad0:    return VK_NUMPAD0;   
	case sf::Keyboard::Numpad1:    return VK_NUMPAD1;   
	case sf::Keyboard::Numpad2:    return VK_NUMPAD2;   
	case sf::Keyboard::Numpad3:    return VK_NUMPAD3;   
	case sf::Keyboard::Numpad4:    return VK_NUMPAD4;   
	case sf::Keyboard::Numpad5:    return VK_NUMPAD5;   
	case sf::Keyboard::Numpad6:    return VK_NUMPAD6;   
	case sf::Keyboard::Numpad7:    return VK_NUMPAD7;   
	case sf::Keyboard::Numpad8:    return VK_NUMPAD8;   
	case sf::Keyboard::Numpad9:    return VK_NUMPAD9;   
	case sf::Keyboard::F1:         return VK_F1;        
	case sf::Keyboard::F2:         return VK_F2;        
	case sf::Keyboard::F3:         return VK_F3;        
	case sf::Keyboard::F4:         return VK_F4;        
	case sf::Keyboard::F5:         return VK_F5;        
	case sf::Keyboard::F6:         return VK_F6;        
	case sf::Keyboard::F7:         return VK_F7;        
	case sf::Keyboard::F8:         return VK_F8;        
	case sf::Keyboard::F9:         return VK_F9;        
	case sf::Keyboard::F10:        return VK_F10;       
	case sf::Keyboard::F11:        return VK_F11;       
	case sf::Keyboard::F12:        return VK_F12;       
	case sf::Keyboard::F13:        return VK_F13;       
	case sf::Keyboard::F14:        return VK_F14;       
	case sf::Keyboard::F15:        return VK_F15;       
	case sf::Keyboard::Pause:      return VK_PAUSE;     
	}
}

