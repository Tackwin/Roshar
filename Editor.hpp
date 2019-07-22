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
		Dispenser,
		Next_Zone,
		Dry_Zone,
		Rock,
		Size
	};
	std::optional<Creating_Element> element_to_create;

	bool require_dragging{ false };
	bool placing_player{ false };
	bool element_creating{ false };

	std::optional<Vector2f> start_selection;

	float sin_time{ 0.f };

	bool snap_horizontal{ false };
	bool snap_vertical{ false };

	bool ask_to_save{ false };

	std::string save_path;

	void delete_all_selected() noexcept;

	void save(const std::filesystem::path& path) noexcept;
public:
	Level* level_to_edit{ nullptr };

	void render(sf::RenderTarget& target) noexcept;
	void update(float dt) noexcept;

	void end_drag(Vector2f start, Vector2f end) noexcept;
};

