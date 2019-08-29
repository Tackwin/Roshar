#include "Editor.hpp"

#include "imgui.h"
#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"
#include "OS/file.hpp"

#include "Level.hpp"
#include "Game.hpp"

#include <cstdio>
#include <array>
#include <algorithm>

#include "Collision.hpp"

void Editor::render(render::Orders& target) noexcept {
	ImGui::Begin("Editor");
	defer{ ImGui::End(); };

	ImGui::InputFloat("Snap grid", &snap_grid);

	if (ImGui::Button("Creating an element")) {
		element_creating = true;
		require_dragging = true;
		if (!element_to_create) element_to_create = (Creating_Element)0;
	}

	ImGui::Separator();

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (ImGui::Button("Open")) {
		file::open_dir_async(
			[&s = save_path](std::optional<std::filesystem::path> path) {
				std::lock_guard guard{ Main_Mutex };
				if (path) s = path->string();
			}
		);
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (ImGui::Button("Save")) {
		if (ask_to_save) ImGui::OpenPopup("Sure ?");
		else save(save_path);
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (ImGui::Button("Load")) {
		file::OpenFileOpts opts;
		opts.allow_multiple = false;
		opts.ext_filters["all"] = { "*.json" };
		file::open_file_async(
			[&s = save_path](file::OpenFileResult result) {
				if (!result.succeded) return;
				auto opt_dyn = load_from_json_file(result.filepath);
				if (!opt_dyn) return;

				std::lock_guard guard{ Main_Mutex };

				game->to_swap_level = (Level)* opt_dyn;
				game->to_swap_level->save_path = result.filepath;

				s = result.filepath.string();
			},
			opts
		);
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (ImGui::Button("Reload")) {
		auto old_markers = std::move(game->current_level.markers);
		auto opt_dyn = load_from_json_file(save_path);
		if (!opt_dyn) return;
		Level new_level = (Level)* opt_dyn;
		new_level.save_path = save_path;

		game->to_swap_level = std::move(new_level);
		game->current_level.markers = std::move(old_markers);
	}
	ImGui::SameLine();
	if (ImGui::Button("Next level") && game->current_level.next_zones.size() > 0) {
		auto opt_dyn = load_from_json_file(game->current_level.next_zones[0].next_level);
		if (!opt_dyn) return;
		Level new_level = (Level)* opt_dyn;
		new_level.save_path = game->current_level.next_zones[0].next_level;

		game->to_swap_level = std::move(new_level);
	}
	char buffer[512];
	strcpy_s(buffer, save_path.data());
	ImGui::PushItemWidth(ImGui::GetWindowWidth());
	ImGui::InputText("", buffer, 512);
	save_path = buffer;
	ImGui::Separator();
	if (ImGui::Button("Import a texture")) {
		file::OpenFileOpts opts;
		opts.allow_multiple = false;
		opts.ext_filters["images"] = { "*.png" };
		file::OpenFileResult result = file::open_file(opts);

		auto key = asset::Store.load_texture(result.filepath);
		if (!key) {
			printf("Couldn't load your texture.\n");
			return;
		}

		auto& texture = asset::Store.textures.at(*key);

		Decor_Sprite decor;
		decor.texture_key = *key;
		decor.texture_loaded = true;
		decor.texture_path = std::filesystem::canonical(result.filepath);
		decor.rec.pos = game->camera.center();
		decor.rec.size = { (float)texture.asset.get_size().x, (float)texture.asset.get_size().y };
		decor.rec.pos -= decor.rec.size / 2;
		game->current_level.decor_sprites.push_back(decor);
	}

	if (ImGui::Checkbox("Edit texture", &edit_texture) && edit_texture) {
		placing_player = false;
		element_creating = false;
		require_dragging = false;
	}

	ImGui::Separator();
	ImGui::Text("Selected");
	if (ImGui::Button("Delete (Suppr)")) delete_all_selected();
	if (ImGui::Button("Set Camera Bound")) set_camera_bound();

	placing_player |= ImGui::Button("Place player");
	element_creating &= !placing_player;
	require_dragging &= !placing_player;
	require_dragging &= element_creating;
	require_dragging |= edit_texture;

	auto pred = [](auto& x) { return x.editor_selected; };
	auto n_selected = std::count_if(BEG_END(game->current_level.prest_sources), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Prest sources");
		float x = { 0 };
		if (n_selected == 1)
			x = std::find_if(BEG_END(game->current_level.prest_sources), pred)->prest;

		ImGui::Text("Prest");
		ImGui::SameLine();
		if (ImGui::InputFloat("Prest", &x))
			for (auto& y : game->current_level.prest_sources) if (pred(y)) y.prest = x;
	}

	n_selected = std::count_if(BEG_END(game->current_level.friction_zones), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Friction zones");
		float friction = { 0 };
		if (n_selected == 1)
			friction = std::find_if(BEG_END(game->current_level.friction_zones), pred)->friction;

		ImGui::Text("Friction");
		ImGui::SameLine();
		if (ImGui::InputFloat("Friction", &friction))
			for (auto& y : game->current_level.friction_zones) if (pred(y)) y.friction = friction;
	}

	n_selected = std::count_if(BEG_END(game->current_level.blocks), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Blocks");
		Block::Kind kind = { Block::Kind::Normal };
		if (n_selected == 1)
			kind = std::find_if(BEG_END(game->current_level.blocks), pred)->kind;
		
		ImGui::Text("kind");
		ImGui::SameLine();
		int k = (int)kind;
		auto act = ImGui::ListBox("Kind", &k, [](void*, int i, const char** out) {
				switch ((Block::Kind)i) {
				case Block::Kind::Normal:       *out = "Normal";    break;
				case Block::Kind::Saturated:    *out = "Saturated"; break;
				case Block::Kind::Eponge:       *out = "Eponge";    break;
				default: assert("Logic error"); *out = "";          break;
				}
				return true;
			},
			nullptr,
			(int)Block::Kind::Count
		);

		if (act) for (auto& x : game->current_level.blocks) if (pred(x)) x.kind = (Block::Kind)k;
	}

	n_selected = std::count_if(BEG_END(game->current_level.key_items), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Key items");
		std::uint64_t id = { 0 };
		if (n_selected == 1)
			id = std::find_if(BEG_END(game->current_level.key_items), pred)->id;

		ImGui::Text("Id");
		ImGui::SameLine();
		int x = (int)id;
		if (ImGui::InputInt("Id", &x))
			for (auto& y : game->current_level.key_items) if (pred(y)) y.id = x;
	}

	n_selected = std::count_if(BEG_END(game->current_level.doors), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Doors");
		bool closed = { true };
		if (n_selected == 1)
			closed = std::find_if(BEG_END(game->current_level.doors), pred)->closed;

		ImGui::Text("Closed");
		ImGui::SameLine();
		if (ImGui::Checkbox("Closed", &closed))
			for (auto& y : game->current_level.doors) if (pred(y)) y.closed = closed;

		thread_local bool must_list{ false };
		ImGui::Checkbox(must_list ? "must list" : "mustnt_list", &must_list);

		if (ImGui::Button("Add Selection to list")) {
			for (auto& y : game->current_level.doors) {
				if (!pred(y)) continue;
				auto& list = must_list ? y.must_triggered : y.mustnt_triggered;

				for (const auto& z : game->current_level.trigger_zones) {
					if (pred(z)) list.push_back(z.id);
				}

				auto& key_list = y.must_have_keys;

				for (const auto& z : game->current_level.key_items) {
					if (pred(z)) key_list.push_back(z.id);
				}

				list.erase(std::unique(BEG_END(list)), END(list));
				key_list.erase(std::unique(BEG_END(key_list)), END(key_list));
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove selection from list")) {
			for (auto& y : game->current_level.doors) {
				if (!pred(y)) continue;
				auto& list = must_list ? y.must_triggered : y.mustnt_triggered;
				auto& key_list = y.must_have_keys;

				for (const auto& z : game->current_level.trigger_zones) {
					if (!pred(z)) continue;
					if (auto it = std::find(BEG_END(list), z.id); it != END(list)) list.erase(it);
				}
				
				for (const auto& z : game->current_level.key_items) {
					auto& l = key_list;

					if (!pred(z)) continue;
					if (auto it = std::find(BEG_END(l), z.id); it != END(l)) l.erase(it);
				}
			}
		}
	}

	n_selected = std::count_if(BEG_END(game->current_level.next_zones), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Next zones");
		char next[512];
		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(game->current_level.next_zones), pred);
			strcpy_s(next, 512, it->next_level.c_str());
		}

		ImGui::Text("Next");
		ImGui::SameLine();
		if (ImGui::InputText("Next", next, sizeof(next))) {
			for (auto& y : game->current_level.next_zones) if (pred(y)) y.next_level = next;
		}
	}

	n_selected = std::count_if(BEG_END(game->current_level.auto_binding_zones), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Auto bindings zones");

		float x;
		float y;

		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(game->current_level.auto_binding_zones), pred);
			x = it->binding.x;
			y = it->binding.y;
		}

		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::PushItemWidth(0.7f * ImGui::GetWindowWidth());
		defer{ ImGui::PopItemWidth(); };
		if (ImGui::InputFloat("X", &x, 0.05f))
			for (auto& a : game->current_level.auto_binding_zones) if (pred(a)) a.binding.x = x;
		if (ImGui::InputFloat("Y", &y, 0.05f))
			for (auto& a : game->current_level.auto_binding_zones) if (pred(a)) a.binding.y = y;
	}

	n_selected = std::count_if(BEG_END(game->current_level.rocks), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Rocks");
		float mass;
		float radius;

		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(game->current_level.rocks), pred);
			mass = it->mass;
			radius = it->r;
		}

		ImGui::Text("Mass");
		ImGui::SameLine();
		if (ImGui::InputFloat("Mass", &mass)) {
			for (auto& y : game->current_level.rocks) if (pred(y)) y.mass = mass;
		}
		ImGui::Text("Radius");
		ImGui::SameLine();
		if (ImGui::InputFloat("Radius", &radius)) {
			for (auto& y : game->current_level.rocks) if (pred(y)) y.r = radius;
		}
	}

	n_selected = std::count_if(BEG_END(game->current_level.decor_sprites), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Texture");

		float opacity{ 1 };
		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(game->current_level.decor_sprites), pred);
			opacity = it->opacity;
		}

		ImGui::Text("Opacity");
		ImGui::SameLine();
		if (ImGui::InputFloat("Opacity", &opacity))
			for (auto& x : game->current_level.decor_sprites) if (pred(x)) x.opacity = opacity;
	}

	n_selected = std::count_if(BEG_END(game->current_level.dispensers), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("dispensers");
		float r{ 0 };
		float speed{ 0 };
		float hz{ 0 };
		float offset_timer{ 0.f };

		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(game->current_level.dispensers), pred);
			r = it->proj_r;
			speed = it->proj_speed;
			hz = it->hz;
			offset_timer = it->offset_timer;
		}

		ImGui::Text("Radius");
		ImGui::SameLine();
		if (ImGui::InputFloat("Radius", &r))
			for (auto& y : game->current_level.dispensers) if (pred(y)) y.proj_r = r;

		ImGui::Text("Speed");
		ImGui::SameLine();
		if (ImGui::InputFloat("Speed", &speed))
			for (auto& y : game->current_level.dispensers) if (pred(y)) y.proj_speed = speed;

		ImGui::Text("Cadence");
		ImGui::SameLine();
		if (ImGui::InputFloat("Cadence", &hz))
			for (auto& y : game->current_level.dispensers) if (pred(y)) {
				y.hz = hz;
				y.timer = 1.f / hz;
			}

		ImGui::Text("Offset");
		ImGui::SameLine();
		if (ImGui::InputFloat("Offset", &offset_timer))
			for (auto& y : game->current_level.dispensers) if (pred(y)) {
				y.offset_timer = offset_timer;
			}
	}

	if (element_creating) {
		ImGui::Begin("Element creeation", &element_creating);
		defer{ ImGui::End(); };

		int x = (int)*element_to_create;
		ImGui::ListBox(
			"Element to create",
			&x,
			[](void*, int idx, const char** out) {
				switch ((Creating_Element)idx)
					{
					case Creating_Element::Block:
						*out = "Block";
						break;
					case Creating_Element::Kill_Zone:
						*out = "Kill_Zone";
						break;
					case Creating_Element::Prest:
						*out = "Prest";
						break;
					case Creating_Element::Dispenser:
						*out = "Dispenser";
						break;
					case Creating_Element::Next_Zone:
						*out = "Next_Zone";
						break;
					case Creating_Element::Dry_Zone:
						*out = "Dry_Zone";
						break;
					case Creating_Element::Rock:
						*out = "Rock";
						break;
					case Creating_Element::Door:
						*out = "Door";
						break;
					case Creating_Element::Trigger_Zone:
						*out = "Trigger_Zone";
						break;
					case Creating_Element::Auto_Binding_Zone:
						*out = "Auto_Binding_Zone";
						break;
					case Creating_Element::Friction_Zone:
						*out = "Friction_Zone";
						break;
					case Creating_Element::Key_Item:
						*out = "Key_Item";
						break;
					default:
					assert(false);
					break;
				}
				return true;
			},
			nullptr,
			(int)Creating_Element::Size
        );
		element_to_create = (Creating_Element)x;
		require_dragging =
			element_to_create != Creating_Element::Prest &&
			element_to_create != Creating_Element::Key_Item &&
			element_to_create != Creating_Element::Rock;
	}
    
	if (ImGui::BeginPopup("Sure ?")) {
		defer{ ImGui::EndPopup(); };

		ImGui::Text(
			"Are you sure to save this level at this location:\n%s\n, it will overwrite whatever\
is there.",
			save_path.c_str()
		);
		if (ImGui::Button("Yes")) {
			save(save_path);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No")) ImGui::CloseCurrentPopup();
	}

	if (pos_start_drag && !edit_texture) {

		Vector2f start = *pos_start_drag;
		Vector2f end = get_mouse_pos();

		Rectangle_t rec{ start, end - start };

		target.push_rectangle(rec, { 0.2, 0.5, 0.2, 0.5 });
	}

	if (start_selection) {
		target.push_rectangle(
			*start_selection, get_mouse_pos() - *start_selection, { 0, 1, 0, .5 }
		);
	}

	for (const auto& d : game->current_level.doors) {
		Vector2f start = d.rec.center();

		for (const auto& x : d.must_triggered) {
			auto it = std::find_if(
				BEG_END(game->current_level.trigger_zones), [x](auto& z) { return z.id == x; }
			);
			if (it == END(game->current_level.trigger_zones)) continue;

			Vector2f end = it->rec.center();

			target.push_line(start, end, { 0.1, 1.0, 0.1, 1.0 });
		}

		for (const auto& x : d.mustnt_triggered) {
			auto it = std::find_if(
				BEG_END(game->current_level.trigger_zones), [x](auto& z) { return z.id == x; }
			);
			if (it == END(game->current_level.trigger_zones)) continue;

			Vector2f end = it->rec.center();

			target.push_line(start, end, { 1.0, 0.1, 0.1, 1.0 });
		}

		for (const auto& x : d.must_have_keys) {
			auto it = std::find_if(
				BEG_END(game->current_level.key_items), [x](auto& z) { return z.id == x; }
			);
			if (it == END(game->current_level.key_items)) continue;

			Vector2f end = it->pos;

			target.push_line(start, end, { 0.1, 1.0, 0.1, 1.0 });
		}
	}

	if (snap_grid) {
		auto& cam = game->camera;

		size_t n = (size_t)(std::abs(cam.w) / snap_grid);
		for (size_t x = 0; x <= n; ++x) {
			Vector2f A = cam.center() - cam.size / 2;
			Vector2f B = cam.center() - cam.size / 2;
			A = snap_grid * (Vector2i)(A / snap_grid);
			B = snap_grid * (Vector2i)(B / snap_grid);

			B.y = cam.center().y + cam.h / 2;

			A.x += x * snap_grid;
			B.x += x * snap_grid;

			target.push_line(A, B, { 0.2, 0.2, 0.2, 0.2 });
		}
		n = (size_t)(std::abs(cam.h) / snap_grid);
		for (size_t y = 0; y <= n; ++y) {
			Vector2f A = cam.center() - cam.size / 2;
			Vector2f B = cam.center() - cam.size / 2;
			A = snap_grid * (Vector2i)(A / snap_grid);
			B = snap_grid * (Vector2i)(B / snap_grid);

			B.x = cam.center().x + cam.w / 2;

			A.y -= y * snap_grid;
			B.y -= y * snap_grid;

			target.push_line(A, B, { 0.2, 0.2, 0.2, 0.2 });
		}
	}
}

void Editor::update(float dt) noexcept {
	sin_time += std::cosf(sin_time) * dt;
	if (!IM::isWindowFocused()) return;

	bool shift = IM::isKeyPressed(Keyboard::LSHIFT);
	auto ctrl = IM::isKeyPressed(Keyboard::LCTRL);
	auto scale = IM::getLastScroll();

	if (shift) scale /= 10;

	if (!edit_texture) game->camera = game->camera.zoom(math::scale_zoom(-scale + 1));
	else for (auto& b : game->current_level.decor_sprites) {
		if (b.editor_selected) {
			auto center = b.rec.pos + b.rec.size / 2;
			b.rec.size *= math::scale_zoom(-scale + 1);
			b.rec.setCenter(center);
		}
	}

	if (IM::isKeyJustPressed(Keyboard::F12)) {
		set_camera_bound();
	}

	if (require_dragging) {
		if (IM::isMouseJustPressed(Mouse::Left) && !pos_start_drag) {
			pos_start_drag = get_mouse_pos();
			drag_offset.clear();
			for (auto& b : game->current_level.decor_sprites)
				if (b.editor_selected) drag_offset.push_back(get_mouse_pos() - b.rec.pos);
		}
		if (edit_texture && IM::isMousePressed(Mouse::Left)) {
			size_t i = 0;
			for (auto& b : game->current_level.decor_sprites) {
				if (b.editor_selected) {
					b.rec.pos = get_mouse_pos() - drag_offset[i++];
				}
			}
		}
		if (IM::isMouseJustReleased(Mouse::Left) && pos_start_drag) {
			Vector2f pos_end_drag = get_mouse_pos();
			end_drag(*pos_start_drag, pos_end_drag);
			pos_start_drag = std::nullopt;
		}
	}
	if (element_creating && element_to_create && IM::isMouseJustPressed(Mouse::Left)) {
		switch (*element_to_create) {
			case Creating_Element::Prest: {
				Prest_Source p;
				p.pos = get_mouse_pos();
				p.prest = 1;
				game->current_level.prest_sources.push_back(p);
				break;
			}
			case Creating_Element::Rock: {
				Rock r;
				r.pos = get_mouse_pos();
				r.r = 0.1f;
				r.mass = 1;
				game->current_level.rocks.push_back(r);
				break;
			}
			case Creating_Element::Key_Item: {
				Key_Item r;
				r.pos = get_mouse_pos();
				r.id = 0;
				game->current_level.key_items.push_back(r);
				break;
			}
			default: break;
		}
	}
    
	if (IM::isMousePressed(Mouse::Middle) && !IM::isMouseJustPressed(Mouse::Middle)) {
		game->camera.pos += -1 * IM::getMouseDeltaInView(game->camera);
	}
	if (IM::isMouseJustPressed(Mouse::Left)) {
		if (placing_player) {
			placing_player = false;

			game->current_level.player.pos = get_mouse_pos();
			game->current_level.player.velocity = {};
		}
		if (edit_texture) {
			require_dragging = true;
		}
	}
	if (IM::isMouseJustPressed(Mouse::Right)) {
		start_selection = get_mouse_pos();
	}
	else if (start_selection && IM::isMouseJustReleased(Mouse::Right)) {
		defer{ start_selection.reset(); };
		Rectanglef rec{*start_selection, get_mouse_pos() - *start_selection};

		auto iter = [shift, ctrl, rec](auto& c) noexcept {
			for (auto& b : c) {
				bool orig = (shift && b.editor_selected);

				if (ctrl)	b.editor_selected &= !test(b, rec);
				else		b.editor_selected = orig || test(b, rec);
			}
		};

		iter(game->current_level.rocks);
		iter(game->current_level.doors);
		iter(game->current_level.blocks);
		iter(game->current_level.key_items);
		iter(game->current_level.dry_zones);
		iter(game->current_level.kill_zones);
		iter(game->current_level.next_zones);
		iter(game->current_level.dispensers);
		iter(game->current_level.trigger_zones);
		iter(game->current_level.prest_sources);
		iter(game->current_level.friction_zones);
		iter(game->current_level.auto_binding_zones);

		drag_offset.clear();
		if (edit_texture) {
			for (auto& b : game->current_level.decor_sprites) {
				b.editor_selected = b.rec.intersect(rec);
			}
		}
		else {
			for (auto& b : game->current_level.decor_sprites) b.editor_selected = false;
		}
	}
	if (IM::isKeyJustReleased(Keyboard::DEL)) delete_all_selected();
	snap_vertical = IM::isKeyPressed(Keyboard::LSHIFT) && IM::isKeyPressed(Keyboard::V);
	snap_horizontal = IM::isKeyPressed(Keyboard::LSHIFT) && IM::isKeyPressed(Keyboard::H);
}

Vector2f Editor::get_mouse_pos() const noexcept {
	auto pos = IM::getMousePosInView(game->camera);
	if (snap_grid == 0) return pos;

	return snap_grid * (Vector2i)(pos / snap_grid);
}

void Editor::end_drag(Vector2f start, Vector2f end) noexcept {
	Rectangle_t rec = { start, end - start };

	if (!element_to_create) return;
#define RETURN_IF_AREA_0 if (rec.area() == 0) return

	switch (*element_to_create) {
		case Creating_Element::Block: {
			RETURN_IF_AREA_0;
			Block new_block;
			new_block.pos = start;
			new_block.size = end - start;

			if (new_block.size.x < 0) new_block.pos.x += new_block.size.x;
			if (new_block.size.y < 0) new_block.pos.y += new_block.size.y;
			new_block.size.x = std::abs(new_block.size.x);
			new_block.size.y = std::abs(new_block.size.y);

			game->current_level.blocks.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Kill_Zone: {
			RETURN_IF_AREA_0;
			Kill_Zone new_block;
			new_block.pos = start;
			new_block.size = end - start;

			if (new_block.size.x < 0) new_block.pos.x += new_block.size.x;
			if (new_block.size.y < 0) new_block.pos.y += new_block.size.y;
			new_block.size.x = std::abs(new_block.size.x);
			new_block.size.y = std::abs(new_block.size.y);

			game->current_level.kill_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Next_Zone: {
			RETURN_IF_AREA_0;
			Next_Zone new_block;
			new_block.pos = start;
			new_block.size = end - start;

			if (new_block.size.x < 0) new_block.pos.x += new_block.size.x;
			if (new_block.size.y < 0) new_block.pos.y += new_block.size.y;
			new_block.size.x = std::abs(new_block.size.x);
			new_block.size.y = std::abs(new_block.size.y);

			game->current_level.next_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Dispenser: {
			Dispenser new_block;
			new_block.start_pos = start;
			new_block.end_pos = end;
			new_block.proj_r = .1f;
			new_block.hz = 1;
			new_block.proj_speed = 1.f;
			new_block.timer = 1.f / new_block.hz;

			game->current_level.dispensers.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Dry_Zone: {
			RETURN_IF_AREA_0;
			Dry_Zone new_block;
			new_block.rec = rec;

			game->current_level.dry_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Friction_Zone: {
			RETURN_IF_AREA_0;
			Friction_Zone new_block;
			new_block.rec = rec;

			game->current_level.friction_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Trigger_Zone: {
			RETURN_IF_AREA_0;
			Trigger_Zone new_block;
			new_block.rec = rec;
			new_block.id = xstd::uuid();

			game->current_level.trigger_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Door: {
			RETURN_IF_AREA_0;
			Door new_block;
			new_block.rec = rec;

			game->current_level.doors.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Auto_Binding_Zone: {
			RETURN_IF_AREA_0;
			Auto_Binding_Zone new_block;
			new_block.rec = rec;
			new_block.uuid = xstd::uuid();

			game->current_level.auto_binding_zones.emplace_back(std::move(new_block));
			break;
		}
		default: assert("Logic Error");
	}
}

void Editor::set_camera_bound() noexcept {
	game->current_level.camera_bound = game->camera;
}

void Editor::delete_all_selected() noexcept {
	auto iter = [](auto& iter) {
		for (size_t i = iter.size() - 1; i + 1 > 0; --i) {
			if (iter[i].editor_selected) {
				iter.erase(BEG(iter) + i);
			}
		}
	};

	iter(game->current_level.rocks);
	iter(game->current_level.doors);
	iter(game->current_level.blocks);
	iter(game->current_level.key_items);
	iter(game->current_level.dry_zones);
	iter(game->current_level.kill_zones);
	iter(game->current_level.next_zones);
	iter(game->current_level.dispensers);
	iter(game->current_level.trigger_zones);
	iter(game->current_level.prest_sources);
	iter(game->current_level.decor_sprites);
	iter(game->current_level.friction_zones);
	iter(game->current_level.auto_binding_zones);

#define S(x) game->current_level.x.size()
	auto n_elements =
		S(rocks) +
		S(doors) +
		S(blocks) +
		S(dry_zones) +
		S(key_items) +
		S(kill_zones) +
		S(next_zones) +
		S(trigger_zones) +
		S(prest_sources) +
		S(decor_sprites) +
		S(friction_zones) +
		S(auto_binding_zones) +
		S(dispensers);
#undef S
    
	if (n_elements == 0) ask_to_save = true;
}

void Editor::save(const std::filesystem::path& path) noexcept {
	save_to_json_file((dyn_struct)game->current_level, path);
	ask_to_save = false;
}

