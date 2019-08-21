#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <array>

#include "./../Math/Vector.hpp"

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

	std::array<Action_Info, sf::Keyboard::KeyCount>    key_infos;
	std::array<Action_Info, sf::Mouse::ButtonCount>    mouse_infos;
	std::array<Action_Info, sf::Joystick::ButtonCount> joystick_buttons_infos;

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

	[[nodiscard]] Vector2f mouse_world_pos(sf::View& v) const noexcept;
	[[nodiscard]] bool is_just_released(sf::Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_just_released(sf::Mouse::Button b) const noexcept;
	[[nodiscard]] bool is_just_released(int b              ) const noexcept;
	[[nodiscard]] bool is_just_pressed(sf::Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_just_pressed(sf::Mouse::Button b) const noexcept;
	[[nodiscard]] bool is_just_pressed(int b              ) const noexcept;
	[[nodiscard]] bool is_pressed(sf::Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_pressed(sf::Mouse::Button b) const noexcept;
	[[nodiscard]] bool is_pressed(int x              ) const noexcept;
};

class InputsManager {
private:
	InputsManager();
	~InputsManager();

private:
	inline static std::list<Inputs_Info> records{};
	inline static std::unordered_map<std::uint64_t, std::list<Inputs_Info>> loaded_record{};
public:
	using Input_Iterator = decltype(records)::iterator;

	static decltype(records)::iterator get_iterator() noexcept;

	static std::string nameOfKey(sf::Keyboard::Key) noexcept;

	static void update(float dt);

	static sf::Keyboard::Key getLastKeyPressed() noexcept;

	static bool isTextJustEntered() noexcept;
	static sf::Uint32 getTextEntered() noexcept;

	static bool isLastSequence(
		const std::vector<sf::Keyboard::Key>& keys,
		const std::vector<sf::Keyboard::Key>& modifiers = {}
	) noexcept;
	static bool isLastSequenceJustFinished(
		std::initializer_list<sf::Keyboard::Key> keys,
		std::initializer_list<sf::Keyboard::Key> modifiers = {}
	) noexcept;

	static int countKeyPressed() noexcept;

	static bool isKeyPressed() noexcept;
	static bool isKeyPressed(const int &key) {
		return isKeyPressed(static_cast<sf::Keyboard::Key>(key));
	};
	static bool isKeyJustPressed() noexcept;
	static bool isKeyPressed(const sf::Keyboard::Key &key);

	static bool isKeyJustPressed(const int &key) {
		return isKeyJustPressed(static_cast<sf::Keyboard::Key>(key));
	};
	static bool isKeyJustPressed(const sf::Keyboard::Key &key);
	
	static bool isKeyJustReleased() noexcept;
	static bool isKeyJustReleased(const int &key) {
		return isKeyJustReleased(static_cast<sf::Keyboard::Key>(key));
	};
	static bool isKeyJustReleased(const sf::Keyboard::Key &key);

	static bool isMousePressed(const int &button) {
		return isMousePressed(static_cast<sf::Mouse::Button>(button));
	};
	static bool isMousePressed(const sf::Mouse::Button &button);

	static bool isMouseJustPressed(const int &button) {
		return isMouseJustPressed(static_cast<sf::Mouse::Button>(button));
	};
	static bool isMouseJustPressed(const sf::Mouse::Button &button);
	
	static bool isMouseJustReleased(const int &button) {
		return isMouseJustReleased(static_cast<sf::Mouse::Button>(button));
	};
	static bool isMouseJustReleased(const sf::Mouse::Button &button);

	static bool isWindowFocused() noexcept;

	static float getLastScroll() noexcept;

	static Vector2u getWindowSize() noexcept;
	static Vector2f getMousePosInView(const sf::View& view);
	static Vector2f getMouseScreenPos();
	static Vector2f getMouseScreenDelta() noexcept;
	static Vector2f getMouseDeltaInView(const sf::View& view) noexcept;

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
	static int get_vkey(sf::Keyboard::Key) noexcept;

	// For now i'll put that here, but it needs to be in his own stuff
	// Maybe when i'll make a custom renderer i'll look into matrix, view and whatnot.
	static Vector2f applyInverseView(const sf::View& view, Vector2f p) noexcept;
};

using IM = InputsManager;
using Input_Iterator = std::list<Inputs_Info>::iterator;
