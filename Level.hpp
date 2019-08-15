#pragma once

#include <unordered_set>
#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Math/Rectangle.hpp"
#include "Math/Vector.hpp"
#include "Memory/ValuePtr.hpp"
#include "dyn_struct.hpp"

#include "Player.hpp"

struct Block {
	bool editor_selected{ false };

	Vector2f pos;
	Vector2f size;
	void render(sf::RenderTarget& target) const noexcept;
};

struct Kill_Zone {
	bool editor_selected{ false };
	
	Vector2f pos;
	Vector2f size;
	void render(sf::RenderTarget& target) const noexcept;
};

struct Next_Zone {
	bool editor_selected{ false };
	
	std::string next_level;

	Vector2f pos;
	Vector2f size;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Trigger_Zone {
	bool editor_selected{ false };

	std::uint64_t id;
	Rectanglef rec;
	bool triggered{ false };

	void render(sf::RenderTarget& target) const noexcept;
};

struct Door {
	bool editor_selected{ false };
	
	Rectanglef rec;

	bool closed{ true };

	std::vector<std::uint64_t> must_triggered;
	std::vector<std::uint64_t> mustnt_triggered;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Dry_Zone {
	bool editor_selected{ false };

	Rectanglef rec;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Dispenser {
	bool editor_selected{ false };
	Vector2f start_pos;
	Vector2f end_pos;

	float hz;
	float proj_r;
	float proj_speed;

	float timer{ 0 };
	float offset_timer{ 0 };

	Dispenser() noexcept;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Projectile {
	Vector2f pos;
	Vector2f speed;

	Vector2f end_pos;

	float r;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Prest_Source {
	static const inline auto Radius_Multiplier = 0.2f;

	bool editor_selected{ false };

	float prest;
	Vector2f pos;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Rock {
	bool editor_selected{ false };

	float r;
	Vector2f pos;
	Vector2f velocity;

	std::vector<Vector2f> bindings;
	float mass;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Auto_Binding_Zone {
	bool editor_selected{ false };

	Rectanglef rec;

	Vector2f binding;
	std::uint64_t uuid;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Friction_Zone {
	bool editor_selected{ false };

	Rectanglef rec;

	float friction{ 1 };

	void render(sf::RenderTarget& target) const noexcept;
};

struct Decor_Sprite {
	float opacity{ 1 };
	bool editor_selected{ false };

	mutable sf::Sprite sprite;

	std::filesystem::path texture_path;
	asset::Key texture_key;
	bool texture_loaded{ false };

	Rectanglef rec;

	void render(sf::RenderTarget& target) const noexcept;
};

struct Level {
	// >SEE:
	// >DEBUG:
	struct Debug_Vector {
		Vector2f a;
		Vector2f b;
	};
	std::vector<Debug_Vector> debug_vectors;

	std::vector<Rock>               rocks;
	std::vector<Door>               doors;
	std::vector<Block>              blocks;
	std::vector<Dry_Zone>           dry_zones;
	std::vector<Kill_Zone>          kill_zones;
	std::vector<Next_Zone>          next_zones;
	std::vector<Dispenser>          dispensers;
	std::vector<Projectile>         projectiles;
	std::vector<Trigger_Zone>       trigger_zones;
	std::vector<Prest_Source>       prest_sources;
	std::vector<Friction_Zone>      friction_zones;
	std::vector<Auto_Binding_Zone>  auto_binding_zones;

	std::vector<Vector2f> markers;

	std::filesystem::path save_path;

	float camera_speed{ 10 };
	float camera_idle_radius{ 0.3f };
	sf::View camera;

	Player player;

	Rectanglef camera_bound{ { 0, 0 }, { 0, 0 } };

	double drag_time{ 0.0 };
	std::optional<Vector2f> start_drag;
	float drag_dead_zone{ 50 };
	size_t rock_dragging_i{ 0 };

	std::vector<Decor_Sprite> decor_sprites;

	void input(IM::Input_Iterator record) noexcept;
	void update(float dt) noexcept;
	void render(sf::RenderTarget& target) const noexcept;

	void pause() noexcept;
	void resume() noexcept;

private:
	bool full_test{ false };
	bool in_replay{ false };
	bool in_test{ false };

	std::uint64_t test_record_id;

	std::uint64_t speedrun_clock_start;

	std::optional<Vector2f> camera_target;

	Vector2f mouse_screen_pos;
	Vector2f mouse_world_pos;
	Vector2u window_size;

	void test_collisions(float dt, Vector2f previous_player_pos) noexcept;

	void mouse_start_drag() noexcept;
	void mouse_on_drag() noexcept;

	void update_camera(float dt) noexcept;
};

extern void from_dyn_struct(const dyn_struct& str, Dispenser& block) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Dispenser& block) noexcept;
extern void from_dyn_struct(const dyn_struct& str, Dry_Zone& x) noexcept;
extern void to_dyn_struct(dyn_struct& str, const Dry_Zone& x) noexcept;
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
