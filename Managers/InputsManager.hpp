#pragma once
#include <optional>
#include <vector>
#include <array>

#include "./../Math/Vector.hpp"
#include "Graphic/Graphics.hpp"

struct Keyboard {
	enum Key {
		A = 0,
		Z,
		E,
		R,
		T,
		Y,
		U,
		I,
		O,
		P,
		Q,
		S,
		D,
		F,
		G,
		H,
		J,
		K,
		L,
		M,
		W,
		X,
		C,
		V,
		B,
		N,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		DEL,
		Return,
		Space,
		Escape,
		Quote,
		LSYS,
		LCTRL,
		LALT,
		LSHIFT,
		RSYS,
		RCTRL,
		RALT,
		RSHIFT,
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

struct Joystick_Button {
	enum X360 {
		A = 0,
		B = 1,
		X = 2,
		Y = 3,
		RB = 6,
		Count
	};
};

struct Inputs_Info {
	constexpr static float Joystick_Dead_Zone{ 10 };
	constexpr static std::uint8_t Version{ 1 };

	struct Action_Info {
		bool pressed : 1;
		bool just_pressed : 1;
		bool just_released : 1;
	};

	std::array<Action_Info, Keyboard::Count>    key_infos;
	std::array<Action_Info, Mouse::Count>       mouse_infos;
	std::array<Action_Info, 32>                 joystick_buttons_infos;

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

	[[nodiscard]] Vector2f mouse_world_pos(render::View_Info& v) const noexcept;
	[[nodiscard]] Vector2f mouse_world_pos(Rectanglef& v       ) const noexcept;
	[[nodiscard]] bool is_just_released(Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_just_released(Mouse::Button b) const noexcept;
	[[nodiscard]] bool is_just_released(int b          ) const noexcept;
	[[nodiscard]] bool is_just_pressed(Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_just_pressed(Mouse::Button b) const noexcept;
	[[nodiscard]] bool is_just_pressed(int b          ) const noexcept;
	[[nodiscard]] bool is_pressed(Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_pressed(Mouse::Button b) const noexcept;
	[[nodiscard]] bool is_pressed(int x          ) const noexcept;
};

class InputsManager {
private:
	inline static std::list<Inputs_Info> records{};
	inline static std::unordered_map<std::uint64_t, std::list<Inputs_Info>> loaded_record{};
public:
	using Input_Iterator = decltype(records)::iterator;

	static decltype(records)::iterator get_iterator() noexcept;

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
	static bool isKeyPressed(const int &key) {
		return isKeyPressed(static_cast<Keyboard::Key>(key));
	};
	static bool isKeyJustPressed() noexcept;
	static bool isKeyPressed(const Keyboard::Key &key);

	static bool isKeyJustPressed(const int &key) {
		return isKeyJustPressed(static_cast<Keyboard::Key>(key));
	};
	static bool isKeyJustPressed(const Keyboard::Key &key);
	
	static bool isKeyJustReleased() noexcept;
	static bool isKeyJustReleased(const int &key) {
		return isKeyJustReleased(static_cast<Keyboard::Key>(key));
	};
	static bool isKeyJustReleased(const Keyboard::Key &key);

	static bool isMousePressed(const int &button) {
		return isMousePressed(static_cast<Mouse::Button>(button));
	};
	static bool isMousePressed(const Mouse::Button &button);

	static bool isMouseJustPressed(const int &button) {
		return isMouseJustPressed(static_cast<Mouse::Button>(button));
	};
	static bool isMouseJustPressed(const Mouse::Button &button);
	
	static bool isMouseJustReleased(const int &button) {
		return isMouseJustReleased(static_cast<Mouse::Button>(button));
	};
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
private:
	static Vector2f applyInverseView(const render::View_Info& view, Vector2f p) noexcept;
	static int get_vkey(Keyboard::Key k) noexcept;
	static int get_vkey(Mouse::Button k) noexcept;
};

using IM = InputsManager;
using Input_Iterator = std::list<Inputs_Info>::iterator;
