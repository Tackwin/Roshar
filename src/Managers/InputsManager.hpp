#pragma once
#include <optional>
#include <vector>
#include <array>
#include <list>

#include "./../Math/Vector.hpp"
#include "Graphic/Graphics.hpp"


// >TODO(Tackwin): for now i'm duplicating those structs here and in RealTime.hpp
// because the ones in RealTime.hpp are supposed to be an abstraction of the os
// and the ones here are supposed to be the key mapping to actions (like jump, run etc..)
// When i'll be doing custom and run time key bindings the names here will change.

struct Keyboard {
	enum Key {
		Unknown = -1, ///< Unhandled key
		A = 0,        ///< The A key
		B,            ///< The B key
		C,            ///< The C key
		D,            ///< The D key
		E,            ///< The E key
		F,            ///< The F key
		G,            ///< The G key
		H,            ///< The H key
		I,            ///< The I key
		J,            ///< The J key
		K,            ///< The K key
		L,            ///< The L key
		M,            ///< The M key
		N,            ///< The N key
		O,            ///< The O key
		P,            ///< The P key
		Q,            ///< The Q key
		R,            ///< The R key
		S,            ///< The S key
		T,            ///< The T key
		U,            ///< The U key
		V,            ///< The V key
		W,            ///< The W key
		X,            ///< The X key
		Y,            ///< The Y key
		Z,            ///< The Z key
		Num0,         ///< The 0 key
		Num1,         ///< The 1 key
		Num2,         ///< The 2 key
		Num3,         ///< The 3 key
		Num4,         ///< The 4 key
		Num5,         ///< The 5 key
		Num6,         ///< The 6 key
		Num7,         ///< The 7 key
		Num8,         ///< The 8 key
		Num9,         ///< The 9 key
		Escape,       ///< The Escape key
		LCTRL,        ///< The left Control key
		LSHIFT,       ///< The left Shift key
		LALT,         ///< The left Alt key
		LSYS,         ///< The left OS specific key: window (Windows), apple (MacOS X), ...
		RCTRL,        ///< The right Control key
		RSHIFT,       ///< The right Shift key
		RALT,         ///< The right Alt key
		RSYS,         ///< The right OS specific key: window (Windows), apple (MacOS X), ...
		Menu,         ///< The Menu key
		LBracket,     ///< The [ key
		RBracket,     ///< The ] key
		Semicolon,    ///< The ; key
		Comma,        ///< The , key
		Period,       ///< The . key
		Quote,        ///< The ' key
		Slash,        ///< The / key
		Backslash,    ///< The \ key
		Tilde,        ///< The ~ key
		Equal,        ///< The = key
		Hyphen,       ///< The - key (hyphen)
		Space,        ///< The Space key
		Return,       ///< The Enter/Return keys
		Backspace,    ///< The Backspace key
		Tab,          ///< The Tabulation key
		PageUp,       ///< The Page up key
		PageDown,     ///< The Page down key
		End,          ///< The End key
		Home,         ///< The Home key
		Insert,       ///< The Insert key
		DEL,          ///< The Delete key
		Add,          ///< The + key
		Subtract,     ///< The - key (minus, usually from numpad)
		Multiply,     ///< The * key
		Divide,       ///< The / key
		Left,         ///< Left arrow
		Right,        ///< Right arrow
		Up,           ///< Up arrow
		Down,         ///< Down arrow
		Numpad0,      ///< The numpad 0 key
		Numpad1,      ///< The numpad 1 key
		Numpad2,      ///< The numpad 2 key
		Numpad3,      ///< The numpad 3 key
		Numpad4,      ///< The numpad 4 key
		Numpad5,      ///< The numpad 5 key
		Numpad6,      ///< The numpad 6 key
		Numpad7,      ///< The numpad 7 key
		Numpad8,      ///< The numpad 8 key
		Numpad9,      ///< The numpad 9 key
		F1,           ///< The F1 key
		F2,           ///< The F2 key
		F3,           ///< The F3 key
		F4,           ///< The F4 key
		F5,           ///< The F5 key
		F6,           ///< The F6 key
		F7,           ///< The F7 key
		F8,           ///< The F8 key
		F9,           ///< The F9 key
		F10,          ///< The F10 key
		F11,          ///< The F11 key
		F12,          ///< The F12 key
		F13,          ///< The F13 key
		F14,          ///< The F14 key
		F15,          ///< The F15 key
		Pause,        ///< The Pause key
		Count = 101
	};
};


struct Mouse {
	enum Button {
		Left = 0,
		Right,
		Middle,
		Count = 5
	};
};

struct Joystick {
	enum Button {
		A = 0,
		B,
		X,
		Y,
		DPAD_UP,
		DPAD_DOWN,
		DPAD_LEFT,
		DPAD_RIGHT,
		START,
		BACK,
		LB,
		RB,
		LT,
		RT,
		Count = 32
	};
};

struct Inputs_Info {
	constexpr static std::uint8_t Version{ 2 };

	struct Action_Info {
		bool pressed : 1;
		bool just_pressed : 1;
		bool just_released : 1;
	};

	std::array<Action_Info, Keyboard::Count>        key_infos;
	std::array<Action_Info, Mouse::Count>           mouse_infos;
	std::array<Action_Info, Joystick::Count>        joystick_buttons_infos;

	Vector2f left_joystick;
	Vector2f right_joystick;
	float left_trigger;
	float right_trigger;

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

	[[nodiscard]] Vector2f mouse_world_pos(render::View_Info& v) const noexcept;
	[[nodiscard]] Vector2f mouse_world_pos(Rectanglef& v       ) const noexcept;
	[[nodiscard]] bool is_just_released(Keyboard::Key    k) const noexcept;
	[[nodiscard]] bool is_just_released(Mouse::Button    b) const noexcept;
	[[nodiscard]] bool is_just_released(Joystick::Button b) const noexcept;
	[[nodiscard]] bool is_just_pressed(Keyboard::Key    k) const noexcept;
	[[nodiscard]] bool is_just_pressed(Mouse::Button    b) const noexcept;
	[[nodiscard]] bool is_just_pressed(Joystick::Button b) const noexcept;
	[[nodiscard]] bool is_pressed(Keyboard::Key    k) const noexcept;
	[[nodiscard]] bool is_pressed(Mouse::Button    b) const noexcept;
	[[nodiscard]] bool is_pressed(Joystick::Button x) const noexcept;
};

class InputsManager {
private:
	inline static std::list<Inputs_Info> records{};
	inline static std::unordered_map<std::uint64_t, std::list<Inputs_Info>> loaded_record{};
public:
	using Input_Iterator = decltype(records)::iterator;

	static decltype(records)::iterator get_iterator() noexcept;
	static bool iterator_is_valid() noexcept;

	static void update(float dt);

	static Keyboard::Key getLastKeyPressed() noexcept;

	static bool isLastSequence(
		const std::vector<Keyboard::Key>& keys,
		const std::vector<Keyboard::Key>& modifiers = {}
	) noexcept;
	static bool isLastSequenceJustFinished(
		std::initializer_list<Keyboard::Key> keys,
		std::initializer_list<Keyboard::Key> modifiers = {}
	) noexcept;

	static bool isKeyPressed() noexcept;
	static bool isKeyPressed(const Keyboard::Key &key);
	static bool isKeyJustPressed() noexcept;
	static bool isKeyJustPressed(const Keyboard::Key& key);
	static bool isKeyJustPressed(Joystick::Button key);
	static bool isKeyJustReleased() noexcept;
	static bool isKeyJustReleased(const Keyboard::Key &key);

	static bool isMousePressed(const Mouse::Button &button);
	static bool isMouseJustPressed(const Mouse::Button &button);
	static bool isMouseJustReleased(const Mouse::Button &button);

	static bool isWindowFocused() noexcept;

	static float getLastScroll() noexcept;

	static Vector2u getWindowSize() noexcept;
	static Vector2f getMousePosInView(const Rectanglef& view) noexcept;
	static Vector2f getMousePosInView(const render::View_Info& view) noexcept;
	static Vector2f getMouseScreenPos();
	static Vector2f getMouseScreenDelta() noexcept;
	static Vector2f getMouseDeltaInView(const Rectanglef& view) noexcept;
	static Vector2f getMouseDeltaInView(const render::View_Info& view) noexcept;
	static Vector2f applyInverseView(const Rectanglef& view, Vector2f p) noexcept;
	static Vector2f applyInverseView(const render::View_Info& view, Vector2f p) noexcept;

	static float get_dt() noexcept;

	static bool save_range(
		std::filesystem::path path, Input_Iterator begin, Input_Iterator end
	) noexcept;
	static std::uint64_t load_record(std::filesystem::path path) noexcept;
	static bool load_record_at(std::filesystem::path path, std::uint64_t id) noexcept;
	static void forget_record(std::uint64_t id) noexcept;

	static Input_Iterator begin(std::uint64_t id) noexcept;
	static size_t size(std::uint64_t id) noexcept;
	static Input_Iterator end(std::uint64_t id) noexcept;
};

using IM = InputsManager;
using Input_Iterator = std::list<Inputs_Info>::iterator;
