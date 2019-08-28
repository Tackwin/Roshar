#pragma once
#include <optional>
#include "Math/Vector.hpp"
#include "Graphic/Graphics.hpp"

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
		Trigger_Zone,
		Door,
		Rock,
		Auto_Binding_Zone,
		Friction_Zone,
		Key_Item,
		Size
	};
	std::optional<Creating_Element> element_to_create;

	bool require_dragging{ false };
	bool placing_player{ false };
	bool element_creating{ false };

	std::optional<Vector2f> start_selection;

	float sin_time{ 0.f };

	float snap_grid{ 0.f };

	bool snap_horizontal{ false };
	bool snap_vertical{ false };

	bool ask_to_save{ false };
	bool edit_texture{ false };

	std::vector<Vector2f> drag_offset;


	void delete_all_selected() noexcept;
	void set_camera_bound() noexcept;

	void save(const std::filesystem::path& path) noexcept;

	Vector2f get_mouse_pos() const noexcept;
public:
	std::string save_path;

	void render(render::Orders& target) noexcept;
	void update(float dt) noexcept;

	void end_drag(Vector2f start, Vector2f end) noexcept;
};

