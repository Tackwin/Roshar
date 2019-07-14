#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "Math/Vector.hpp"

struct Level;
struct Block;
class Editor {
	std::optional<Vector2f> pos_start_drag;

	enum class Creating_Element {
		Block = 0,
		Prest,
		Kill_Zone,
		Size
	};
	std::optional<Creating_Element> element_to_create;

	bool require_dragging{ false };
	bool placing_player{ false };
	bool element_creating{ false };

	float sin_time{ 0.f };

	std::string save_path;

	void render_selected(sf::RenderTarget& target, Block& block) noexcept;
	void delete_all_selected() noexcept;
public:
	Level* level_to_edit{ nullptr };

	void render(sf::RenderTarget& target) noexcept;
	void update(float dt) noexcept;

	void end_drag(Vector2f start, Vector2f end) noexcept;
};

