#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <array>

#include "./../Math/Vector.hpp"

struct Inputs_Info {
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

	[[nodiscard]] Vector2f mouse_world_pos(sf::View& v) const noexcept;
	[[nodiscard]] bool is_just_released(sf::Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_just_released(sf::Mouse::Button b) const noexcept;
	[[nodiscard]] bool is_just_pressed(sf::Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_just_pressed(sf::Mouse::Button b) const noexcept;
	[[nodiscard]] bool is_pressed(sf::Keyboard::Key k) const noexcept;
	[[nodiscard]] bool is_pressed(sf::Mouse::Button b) const noexcept;
};

class InputsManager {
private:
	InputsManager();
	~InputsManager();

private:
	static std::list<Inputs_Info> records;
public:
	using Input_Iterator = decltype(records)::iterator;

	static decltype(records)::iterator get_iterator() noexcept;

	static std::string nameOfKey(sf::Keyboard::Key) noexcept;

	static void update(sf::RenderWindow &window, float dt);

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

private:
	// For now i'll put that here, but it needs to be in his own stuff
	// Maybe when i'll make a custom renderer i'll look into matrix, view and whatnot.

	static Vector2f applyInverseView(const sf::View& view, Vector2f p) noexcept;
};

using IM = InputsManager;