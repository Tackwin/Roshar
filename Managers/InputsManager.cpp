#include "InputsManager.hpp"

#include <cassert>

#include "imgui.h"
#include "imgui-SFML.h"


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
[[nodiscard]] bool Inputs_Info::is_just_pressed(sf::Keyboard::Key k) const noexcept {
	return key_infos[k].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_just_pressed(sf::Mouse::Button b) const noexcept {
	return mouse_infos[b].just_pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(sf::Keyboard::Key k) const noexcept {
	return key_infos[k].pressed;
}
[[nodiscard]] bool Inputs_Info::is_pressed(sf::Mouse::Button b) const noexcept {
	return mouse_infos[b].pressed;
}


std::list<Inputs_Info> IM::records;

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

void InputsManager::update(sf::RenderWindow& window, float dt) {
	Inputs_Info new_record = {};

	new_record.dt = dt;

	for (size_t i = 0; i < sf::Keyboard::KeyCount; ++i) {
		auto pressed = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)i);
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

	sf::Event event;
	while(window.pollEvent(event)) {

		ImGui::SFML::ProcessEvent(event);

		if (!window.hasFocus()) continue;
		
		if(event.type == sf::Event::Closed)
			window.close();
		if (event.type == sf::Event::MouseWheelScrolled) {
			new_record.scroll = event.mouseWheelScroll.delta;
		}
	}

	new_record.focused = window.hasFocus();
	Vector2f last_mouse_screen_pos =
		records.empty() ? Vector2f{} : records.back().mouse_screen_pos;

	new_record.mouse_screen_pos = sf::Mouse::getPosition(window);
	new_record.mouse_screen_delta = new_record.mouse_screen_pos - last_mouse_screen_pos;

	new_record.window_size = window.getSize();

	ImGui::SFML::Update(window, sf::seconds(dt));
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

