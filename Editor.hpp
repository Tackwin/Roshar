#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "Math/Vector.hpp"

struct Level;
class Editor {
	std::optional<Vector2f> pos_start_drag;

	enum class Creating_Element {
		Block = 0,
		Size
	};
	std::optional<Creating_Element> element_to_create;

	bool require_dragging{ false };
	bool placing_player{ false };

public:
	Level* level_to_edit{ nullptr };

	void input() noexcept;
	void render(sf::RenderTarget& target) noexcept;
	void update(float dt) noexcept;

	void end_drag(Vector2f start, Vector2f end) noexcept;
};

