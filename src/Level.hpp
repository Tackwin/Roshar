#pragma once

#include <unordered_set>
#include <optional>
#include <vector>

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Math/Rectangle.hpp"
#include "Math/Vector.hpp"
#include "dyn_struct.hpp"
#include "Assets.hpp"

#include "Graphic/Graphics.hpp"

#include "Player.hpp"

#include "introspection.hpp"

struct Editable {
	size_t id;
	bool editor_selected{ false };
	bool editor_remove_flage{ false };

	virtual void imgui_edit() noexcept {};
	virtual void render(render::Orders& target) const noexcept = 0;
};

struct Physicable {
	Rectanglef rec;
};

struct Flowing_Water : Editable {
	std::vector<Vector2f> path;
	float width{ 10.f };
	float flow_rate{ 1.f };
	virtual void render(render::Orders& target) const noexcept;
};

struct Block : Editable, Physicable {
	bool back{ false };

	enum class Prest_Kind {
		Normal = 0,
		Eponge,
		Saturated,
		Count
	} prest_kind{ Prest_Kind::Normal };

	bool destroy_on_step{false};
	bool stepped_on{false};
	float destroy_timer{ 0.f };
	float destroy_time{ 0.f };
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Kill_Zone : Editable, Physicable {
	virtual void render(render::Orders& target) const noexcept;
};
struct Dry_Zone : Editable, Physicable {
	virtual void render(render::Orders& target) const noexcept;
};

struct Next_Zone : Editable, Physicable {
	std::string next_level;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Trigger_Zone : Editable, Physicable {
	bool editor_selected{ false };

	std::uint64_t id;
	bool triggered{ false };
	virtual void render(render::Orders& target) const noexcept;
};

struct Door : Editable, Physicable {
	bool closed{ true };

	std::vector<std::uint64_t> must_triggered;
	std::vector<std::uint64_t> mustnt_triggered;
	std::vector<std::uint64_t> must_have_keys;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Dispenser : Editable, Physicable {
	Vector2f end_pos;

	float hz;
	float proj_r;
	float proj_speed;

	float timer{ 0 };
	float offset_timer{ 0 };

	Dispenser() noexcept;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Projectile : Physicable {
	Vector2f speed;

	Vector2f end_pos;

	float r;
	virtual void render(render::Orders& target) const noexcept;
};

struct Prest_Source : Editable, Physicable {
	static const inline auto Radius_Multiplier = 0.2f;
	float prest;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Rock : Editable, Physicable {
	std::uint64_t running_id = xstd::uuid();

	float r;
	Vector2f velocity;

	std::vector<Vector2f> bindings;
	float mass;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;

	auto operator==(const Rock& other) const {
		return running_id == other.running_id;
	}
};

struct Auto_Binding_Zone : Editable, Physicable {
	Vector2f binding;
	std::uint64_t uuid;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Friction_Zone : Editable, Physicable {
	float friction{ 1 };
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Decor_Sprite : Editable, Physicable {
	float opacity{ 1 };

	std::filesystem::path texture_path;
	asset::Key texture_key;
	bool texture_loaded{ false };
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Key_Item : Editable, Physicable {
	inline static Vector2f Key_World_Size{ 0.16f, 0.36f };
	std::uint64_t id;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Torch : Editable, Physicable {
	Vector4d color;
	float intensity;

	float random_factor;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;
};

struct Moving_Block : Editable, Physicable {
	std::vector<Vector2f> waypoints;

	float speed{ 1 };
	bool looping{ false };

	void update(float dt) noexcept;
	virtual void imgui_edit() noexcept;
	virtual void render(render::Orders& target) const noexcept;

	Vector2f to_move{};

	float t{ 0 };
	float max_t{ 0 };
	bool reverse{ false };

	bool moving_player{false};
};

struct Level {
	// >TODO: level id system.
	inline static auto Start_Level = "assets/levels/1-1.json";

	// >SEE:
	// >DEBUG:
	struct Debug_Vector {
		Vector2f a;
		Vector2f b;
	};
	std::vector<Debug_Vector> debug_vectors;

	std::string name;

	std::vector<Rock>               rocks;
	std::vector<Door>               doors;
	std::vector<Block>              blocks;
	std::vector<Torch>              torches;
	std::vector<Dry_Zone>           dry_zones;
	std::vector<Key_Item>           key_items;
	std::vector<Kill_Zone>          kill_zones;
	std::vector<Next_Zone>          next_zones;
	std::vector<Dispenser>          dispensers;
	std::vector<Projectile>         projectiles;
	std::vector<Moving_Block>       moving_blocks;
	std::vector<Trigger_Zone>       trigger_zones;
	std::vector<Prest_Source>       prest_sources;
	std::vector<Flowing_Water>      flowing_waters;
	std::vector<Friction_Zone>      friction_zones;
	std::vector<Auto_Binding_Zone>  auto_binding_zones;

	std::vector<Particle> particles;

	std::vector<Particle_Spot> particle_spots;

	std::vector<Vector2f> markers;

	std::filesystem::path file_path;
	
	std::optional<size_t> focused_rock;

	size_t phantom_path_idx{ 0 };
	std::vector<std::vector<Player::Graphic_State>> phantom_paths;

	Player player;

	float shake_factor = 0;
	float shake_treshold = 0.5f;
	std::optional<Vector2f> unit_shake;
	Vector2f camera_shake_target;
	float camera_shake_idle_radius{ 0.03f };
	
	Vector2f camera_target;
	float camera_speed{ 10 };
	float camera_idle_radius{ 0.3f };
	Rectanglef camera{ { -64, -36 }, { 128, 72 } };
	Rectanglef ui_camera{
		{ 0.f, 0.f }, { 1.f, Environment.window_height / 1.f * Environment.window_width }
	};
	//Rectanglef camera_bound{ { 0, 0 }, { 0, 0 } };
	//Rectanglef camera_start;

	Vector4d ambient_color{ 1, 1, 1, 1 };
	float ambient_intensity{ 1 };

	float score_timer{ 0.f };

	std::vector<Decor_Sprite> decor_sprites;

	void input(IM::Input_Iterator record) noexcept;
	void update(float dt) noexcept;
	void render_debug(render::Orders& target) const noexcept;
	void render(render::Orders& target) const noexcept;

	void update_camera(float dt) noexcept;

	void pause() noexcept;
	void resume() noexcept;

	void bind_rock(std::uint64_t x, Vector2f bind) noexcept;

	void feed_phantom_path(std::vector<std::vector<Player::Graphic_State>> p) noexcept;
private:
	Vector2f mouse_screen_pos;
	Vector2f mouse_world_pos;
	Vector2u window_size;

	void test_collisions(float dt) noexcept;

	void update_player(float dt) noexcept;
};

extern void from_dyn_struct(const dyn_struct& str, Dispenser& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Dispenser& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Moving_Block& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Moving_Block& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Torch& light) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Torch& light) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Dry_Zone& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Dry_Zone& x) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Key_Item& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Key_Item& x) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Friction_Zone& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Friction_Zone& x) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Next_Zone& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Next_Zone& x) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Block& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Block& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Kill_Zone& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Kill_Zone& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Prest_Source& prest) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Prest_Source& prest) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Rock& r) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Rock& r) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Trigger_Zone& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Trigger_Zone& x) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Door& d) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Door& d) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Decor_Sprite& level) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Decor_Sprite& level) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Auto_Binding_Zone& level) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Auto_Binding_Zone& level) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Level& level) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Level& level) noexcept;
