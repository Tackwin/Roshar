#include "Editor.hpp"

#include <cstdio>
#include <array>
#include <algorithm>

#include "imgui.h"

#include "Level.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/EntityManager.hpp"
#include "Managers/AssetsManager.hpp"

#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"

#include "OS/file.hpp"

#include "Game.hpp"

#include "Collision.hpp"

void Editor::render_imgui() noexcept {
		ImGui::Begin("Editor");
	defer{ ImGui::End(); };

	ImGui::InputFloat("Snap grid", &snap_grid);

	if (ImGui::Button("Creating an element")) {
		element_creating = true;
		require_dragging = true;
		if (!element_to_create) element_to_create = (Creating_Element)0;
	}

	if (ImGui::CollapsingHeader("Level parameter")) {
		Vector4f x = (Vector4f)game->play_screen.current_level.ambient_color;
		ImGui::ColorPicker4("Ambient Color", &x.x);
		ImGui::SliderFloat(
			"Ambient Intensity", &game->play_screen.current_level.ambient_intensity, 0, 1
			);
		game->play_screen.current_level.ambient_color = (Vector4d)x;
	}

	if (ImGui::CollapsingHeader("Operations")) {
		if (ImGui::Button("Load/Save every levels")) {
			file::open_dir_async([](std::optional<std::filesystem::path> path) {
				if (!path) return;
				for (auto& f : std::filesystem::recursive_directory_iterator(*path)) {
					if (!f.is_regular_file() || f.path().extension() != ".json") continue;
					printf("Loading %s... ", f.path().generic_string().c_str());
					defer{ printf("\n"); };

					auto opt_dyn = load_from_json_file(f.path());
					if (!opt_dyn) continue;
					printf("Saving... ");
					save_to_json_file((dyn_struct)((Level)*opt_dyn), f.path());
					printf(" OK !");
				}
			});
		}
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

				game->play_screen.to_swap_level = (Level)* opt_dyn;
				game->play_screen.to_swap_level->file_path = result.filepath;

				s = result.filepath.string();
			},
			opts
		);
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (ImGui::Button("Reload")) {
		auto old_markers = std::move(game->play_screen.current_level.markers);
		auto opt_dyn = load_from_json_file(save_path);
		if (!opt_dyn) return;
		Level new_level = (Level)* opt_dyn;
		new_level.file_path = save_path;

		game->play_screen.to_swap_level = std::move(new_level);
		game->play_screen.current_level.markers = std::move(old_markers);
	}
	ImGui::SameLine();
	if (ImGui::Button("Next level") && game->play_screen.current_level.next_zones.size() > 0) {
		auto next_level_path =
			game->play_screen.current_level.file_path.parent_path() /
			game->play_screen.current_level.next_zones[0].next_level;

		auto opt_dyn = load_from_json_file(next_level_path);
		if (!opt_dyn) return;
		Level new_level = (Level)* opt_dyn;
		new_level.file_path = next_level_path;

		game->play_screen.to_swap_level = std::move(new_level);
	}
	char buffer[512];
	strcpy(buffer, save_path.data());
	ImGui::PushItemWidth(ImGui::GetWindowWidth());
	ImGui::InputText("Save path: ", buffer, 512);
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
		decor.texture_path = std::filesystem::weakly_canonical(result.filepath);
		decor.rec.pos = game->play_screen.current_level.camera.center();
		decor.rec.size = {
			(float)texture.albedo.get_size().x, (float)texture.albedo.get_size().y
		};
		decor.rec.pos -= decor.rec.size / 2;
		game->play_screen.current_level.decor_sprites.push_back(decor);
	}

	if (ImGui::Checkbox("Edit texture", &edit_texture) && edit_texture) {
		placing_player = false;
		element_creating = false;
		require_dragging = false;
	}

	ImGui::Separator();
	if (game->play_screen.current_level.name.empty()) {
		game->play_screen.current_level.name = save_path;
	}
	strcpy(buffer, game->play_screen.current_level.name.data());
	if (ImGui::InputText("Name:", buffer, 512)) {
		game->play_screen.current_level.name = buffer;
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

	bool flag;

	#define X(T, t)\
	if constexpr (std::is_base_of_v<Editable, T>) {\
		flag = false;\
		for (auto& x : game->play_screen.current_level. t) if (((Editable*)&x)->editor_selected) {\
			if (!flag) {\
				ImGui::Separator();\
				ImGui::Text(#T);\
			}\
			ImGui::PushID(&x);\
			((Editable*)&x)->imgui_edit();\
			ImGui::PopID();\
			flag = true;\
		}\
	}
	LIST_ENTITIES
	#undef X

	if (element_creating) {
		ImGui::Begin("Element creeation", &element_creating);
		defer{ ImGui::End(); };

		int x = (int)*element_to_create;
		ImGui::ListBox(
			"Element to create",
			&x,
			[](void*, int idx, const char** out) {
				#define X(x) case Creating_Element::x: *out = #x; break
				switch ((Creating_Element)idx) {
					X(Block);
					X(Kill_Zone);
					X(Prest);
					X(Dispenser);
					X(Next_Zone);
					X(Dry_Zone);
					X(Rock);
					X(Door);
					X(Trigger_Zone);
					X(Auto_Binding_Zone);
					X(Friction_Zone);
					X(Key_Item);
					X(Torch);
					X(Flowing_Water);
					X(Moving_Block);
					default: assert(false);
					break;
				}
				#undef X
				return true;
			},
			nullptr,
			(int)Creating_Element::Size
        );
		element_to_create = (Creating_Element)x;
		require_dragging =
			element_to_create != Creating_Element::Flowing_Water &&
			element_to_create != Creating_Element::Prest &&
			element_to_create != Creating_Element::Key_Item &&
			element_to_create != Creating_Element::Rock &&
			element_to_create != Creating_Element::Torch;
	}

	if (ImGui::Button("Resave every levels in dir")) {
		file::open_dir_async([](std::optional<std::filesystem::path> opt_path) {
			if (!opt_path) return;
			auto& path = *opt_path;

			for (auto& p : std::filesystem::recursive_directory_iterator(path)) {
				if (!std::filesystem::is_regular_file(p)) continue;
				auto ext = p.path().extension();
				if (ext != ".json") continue;

				printf("Resaving: %s... ", p.path().generic_string().c_str());
				if (auto l = load_from_json_file(p); l) {
					save_to_json_file((Level)(*l), p);
				}
				printf("Done.\n");
			}
		});
	}

	if (ImGui::BeginPopup("Sure ?")) {
		defer{ ImGui::EndPopup(); };

		ImGui::Text(
			"Are you sure to save this level at this location:\n%s\n, it will overwrite whatever"
			"is there.",
			save_path.c_str()
		);
		if (ImGui::Button("Yes")) {
			save(save_path);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("No")) ImGui::CloseCurrentPopup();
	}
}

void Editor::render(render::Orders& target) noexcept {
	render_imgui();

	if (pos_start_drag && !edit_texture) {

		Vector2f start = *pos_start_drag;
		Vector2f end = get_mouse_pos();

		Rectangle_t<float> rec{ start, end - start };
		rec.size += snap_grid;

		target.push_rectangle(rec, { 0.2, 0.5, 0.2, 0.5 });
	}

	if (start_selection) {
		auto rec = Rectanglef{ *start_selection, get_mouse_pos() - *start_selection };
		rec.size += snap_grid;

		target.push_rec(rec, { 0, 1, 0, .5 });
	}

	for (const auto& d : game->play_screen.current_level.doors) {
		Vector2f start = d.rec.center();

		for (const auto& x : d.must_triggered) {
			auto it = std::find_if(
				BEG_END(game->play_screen.current_level.trigger_zones),
				[x](auto& z) { return z.id == x; }
			);
			if (it == END(game->play_screen.current_level.trigger_zones)) continue;

			Vector2f end = it->rec.center();

			target.push_line(start, end, { 0.1, 1.0, 0.1, 1.0 }, .01f);
		}

		for (const auto& x : d.mustnt_triggered) {
			auto it = std::find_if(
				BEG_END(game->play_screen.current_level.trigger_zones),
				[x](auto& z) { return z.id == x; }
			);
			if (it == END(game->play_screen.current_level.trigger_zones)) continue;

			Vector2f end = it->rec.center();

			target.push_line(start, end, { 1.0, 0.1, 0.1, 1.0 }, .01f);
		}

		for (const auto& x : d.must_have_keys) {
			auto it = std::find_if(
				BEG_END(game->play_screen.current_level.key_items),
				[x](auto& z) { return z.id == x; }
			);
			if (it == END(game->play_screen.current_level.key_items)) continue;

			Vector2f end = it->rec.pos;

			target.push_line(start, end, { 0.1, 1.0, 0.1, 1.0 }, .01f);
		}
	}

	auto& cam = game->play_screen.current_level.camera;
	for (const auto& x : game->play_screen.current_level.moving_blocks) {
		float pixel = cam.w / Environment.window_width;
		for (size_t i = 1; i < x.waypoints.size(); ++i)
			target.push_line(x.waypoints[i - 1], x.waypoints[i], { 1, 1, 1, 0.8 }, pixel);

		for (const auto& y : x.waypoints) target.push_circle(0.1f, y, { 0.8, 0.8, 1.0, 1.0 });
	}

	if (snap_grid) {

		target.late_push_view(cam);
		defer{ target.late_pop_view(); };

		Vector2f row = snap_grid * (Vector2i)((cam.pos / snap_grid).apply(
			[](auto x) { return std::roundf(x); })
		);
		Vector2f col = snap_grid * (Vector2i)((cam.pos / snap_grid).apply(
			[](auto x) { return std::roundf(x); })
		);
		Vector4d color = { .6, .6, .6, .6 };

		while (row.x < cam.x + cam.w) {
			target.late_push_line(
				row,
				row + Vector2f{ 0, cam.h * 2 },
				color,
				cam.w / Environment.window_width
			);

			row.x += snap_grid;
		}
		while (col.y < cam.y + cam.h) {
			target.late_push_line(
				col,
				col + Vector2f{ cam.w * 2, 0},
				color,
				cam.h / Environment.window_height
			);

			col.y += snap_grid;
		}

		target.late_push_rec({get_mouse_pos(), V2F(snap_grid)}, color);
	}

	if (pos_modifier) {
		Rectanglef buffer_view{
			{ 0, 0 }, { 1.f * Environment.buffer_width, 1.f * Environment.buffer_height }
		};
		Rectanglef screen_view{
			0,
			0,
			1.f * Environment.inner_window_size_width,
			1.f * Environment.inner_window_size_height
		};
		target.late_push_view(buffer_view);
		defer { target.late_pop_view(); };

		auto cam = game->play_screen.current_level.camera;
		auto pos = IM::view_to_buffer(cam, *pos_modifier);
		auto screen_pos =
			IM::pos_view_to_window(cam, *pos_modifier);

		auto x_size = IM::size_view_to_view(buffer_view, screen_view, {300, 30});
		auto y_size = IM::size_view_to_view(buffer_view, screen_view, {30, 300});

		bool x_hover = Rectanglef{screen_pos, x_size}.in(IM::getMouseScreenPos());
		bool y_hover = Rectanglef{screen_pos, y_size}.in(IM::getMouseScreenPos());

		target.late_push_rec(
			{pos, {300, 30}}, x_hover ? Vector4d{.9, .9, .9, 1} : Vector4d{1, 0, 0, 1}
		);
		target.late_push_rec(
			{pos, {30, 300}}, y_hover ? Vector4d{.9, .9, .9, 1} : Vector4d{1, 0, 0, 1}
		);
	}
}

void Editor::update(float dt) noexcept {
	sin_time += std::cosf(sin_time) * dt;
	if (!IM::isWindowFocused()) return;

	bool shift = IM::isKeyPressed(Keyboard::LSHIFT);
	auto ctrl = IM::isKeyPressed(Keyboard::LCTRL);
	auto scale = IM::getLastScroll();

	if (shift) scale /= 10;

	if (!edit_texture) {
		auto& cam = game->play_screen.current_level.camera;
		cam = cam.zoom(math::scale_zoom(-scale + 1));
	}
	else for (auto& b : game->play_screen.current_level.decor_sprites) {
		if (b.editor_selected) {
			auto center = b.rec.pos + b.rec.size / 2;
			b.rec.size *= math::scale_zoom(-scale + 1);
			b.rec.setCenter(center);
		}
	}

	if (IM::isKeyJustPressed(Keyboard::F12)) {
		set_camera_bound();
	}

	if (IM::isKeyJustPressed(Keyboard::A)) {
		for (size_t i = game->play_screen.current_level.moving_blocks.size() - 1; i + 1 > 0; --i) {
			auto& x = game->play_screen.current_level.moving_blocks[i];
			if (!x.editor_selected) continue;

			if (IM::isKeyPressed(Keyboard::LSHIFT) || IM::isKeyPressed(Keyboard::RSHIFT)) {
				if (!x.waypoints.empty()) x.waypoints.pop_back();
			} else {
				x.waypoints.push_back(get_mouse_pos());
			}

			if (x.waypoints.empty()) game->play_screen.current_level.moving_blocks.erase(
				BEG(game->play_screen.current_level.moving_blocks) + i
			);

			if (x.waypoints.size() > 1)
				x.max_t += x.waypoints.back().dist_to(*(END(x.waypoints) - 2));
		}
		
		for (size_t i = game->play_screen.current_level.flowing_waters.size() - 1; i + 1 > 0; --i) {
			auto& x = game->play_screen.current_level.flowing_waters[i];
			if (!x.editor_selected) continue;

			if (IM::isKeyPressed(Keyboard::LSHIFT) || IM::isKeyPressed(Keyboard::RSHIFT)) {
				if (!x.path.empty()) x.path.pop_back();
			} else {
				x.path.push_back(get_mouse_pos());
			}

			
			if (x.path.empty()) game->play_screen.current_level.flowing_waters.erase(
				BEG(game->play_screen.current_level.flowing_waters) + i
			);
		}
	}

	if (require_dragging) {
		if (IM::isMouseJustPressed(Mouse::Left) && !pos_start_drag) {
			pos_start_drag = get_mouse_pos();
			drag_offset.clear();
			for (auto& b : game->play_screen.current_level.decor_sprites)
				if (b.editor_selected) drag_offset.push_back(get_mouse_pos() - b.rec.pos);
		}
		if (edit_texture && IM::isMousePressed(Mouse::Left)) {
			size_t i = 0;
			for (auto& b : game->play_screen.current_level.decor_sprites) {
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
			case Creating_Element::Flowing_Water: {
				Flowing_Water w;
				w.path.push_back(get_mouse_pos());
				game->play_screen.current_level.flowing_waters.push_back(w);
				break;
			}
			case Creating_Element::Prest: {
				Prest_Source p;
				p.rec.pos = get_mouse_pos();
				p.prest = 1;
				game->play_screen.current_level.prest_sources.push_back(p);
				break;
			}
			case Creating_Element::Rock: {
				Rock r;
				r.rec.pos = get_mouse_pos();
				r.r = 0.1f;
				r.mass = 1;
				game->play_screen.current_level.rocks.push_back(r);
				break;
			}
			case Creating_Element::Key_Item: {
				Key_Item r;
				r.rec.pos = get_mouse_pos();
				r.id = 0;
				game->play_screen.current_level.key_items.push_back(r);
				break;
			}
			case Creating_Element::Torch: {
				Torch r;
				r.rec.pos = get_mouse_pos();
				r.intensity = 0.5;
				r.color = { 1, 1, 1, 1 };
				r.random_factor = 0;
				game->play_screen.current_level.torches.push_back(r);
				break;
			}
			default: break;
		}
	}

	if (IM::isMousePressed(Mouse::Middle) && !IM::isMouseJustPressed(Mouse::Middle)) {
		game->play_screen.current_level.camera.pos +=
			-1 * IM::getMouseDeltaInView(game->play_screen.current_level.camera);
	}
	if (IM::isMouseJustPressed(Mouse::Left)) {
		if (placing_player) {
			placing_player = false;

			game->play_screen.current_level.player.hitbox.pos = get_mouse_pos();
			game->play_screen.current_level.player.velocity = {};
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
				Editable* edit = (Editable*)&b;
				bool orig = (shift && edit->editor_selected);

				if (ctrl)	edit->editor_selected &= !test(b, rec);
				else		edit->editor_selected = orig || test(b, rec);
			}
		};

		#define X(T, t)\
			if constexpr (std::is_base_of_v<Editable, T>) iter(game->play_screen.current_level. t);
		LIST_ENTITIES;
		#undef X
		// The collision detection for the camera fixed point is different.
		// >TODO(Tackwin)

		drag_offset.clear();
		if (edit_texture) {
			for (auto& b : game->play_screen.current_level.decor_sprites) {
				b.editor_selected = b.rec.intersect(rec);
			}
		}
		else {
			for (auto& b : game->play_screen.current_level.decor_sprites) b.editor_selected = false;
		}
	}
	if (IM::isKeyJustReleased(Keyboard::DEL)) delete_all_selected();
	snap_vertical = IM::isKeyPressed(Keyboard::LSHIFT) && IM::isKeyPressed(Keyboard::V);
	snap_horizontal = IM::isKeyPressed(Keyboard::LSHIFT) && IM::isKeyPressed(Keyboard::H);

	update_selected_pos();
}

Vector2f Editor::get_mouse_pos() const noexcept {
	auto pos = IM::getMousePosInView(game->play_screen.current_level.camera);
	if (snap_grid == 0) return pos;

	Vector2f result;
	result.x = snap_grid * (std::floor(pos.x / snap_grid));
	result.y = snap_grid * (std::floor(pos.y / snap_grid));

	return result;
}

void Editor::end_drag(Vector2f start, Vector2f end) noexcept {
	Rectangle_t<float> rec = { start, end - start };
	rec.size += snap_grid;

	if (!element_to_create) return;
#define RETURN_IF_AREA_0 if (start == end) return

	switch (*element_to_create) {
		case Creating_Element::Block: {
			RETURN_IF_AREA_0;
			Block new_block;
			new_block.rec = rec;

			game->play_screen.current_level.blocks.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Kill_Zone: {
			RETURN_IF_AREA_0;
			Kill_Zone new_block;
			new_block.rec = rec;

			game->play_screen.current_level.kill_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Next_Zone: {
			RETURN_IF_AREA_0;
			Next_Zone new_block;
			new_block.rec = rec;

			game->play_screen.current_level.next_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Dispenser: {
			Dispenser new_block;
			new_block.rec.pos = start;
			new_block.end_pos = end;
			new_block.proj_r = .1f;
			new_block.hz = 1;
			new_block.proj_speed = 1.f;
			new_block.timer = 1.f / new_block.hz;

			game->play_screen.current_level.dispensers.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Dry_Zone: {
			RETURN_IF_AREA_0;
			Dry_Zone new_block;
			new_block.rec = rec;

			game->play_screen.current_level.dry_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Friction_Zone: {
			RETURN_IF_AREA_0;
			Friction_Zone new_block;
			new_block.rec = rec;

			game->play_screen.current_level.friction_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Trigger_Zone: {
			RETURN_IF_AREA_0;
			Trigger_Zone new_block;
			new_block.rec = rec;
			new_block.id = xstd::uuid();

			game->play_screen.current_level.trigger_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Door: {
			RETURN_IF_AREA_0;
			Door new_block;
			new_block.rec = rec;

			game->play_screen.current_level.doors.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Moving_Block: {
			RETURN_IF_AREA_0;
			Moving_Block new_block;
			new_block.rec = rec;

			game->play_screen.current_level.moving_blocks.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Auto_Binding_Zone: {
			RETURN_IF_AREA_0;
			Auto_Binding_Zone new_block;
			new_block.rec = rec;
			new_block.uuid = xstd::uuid();

			game->play_screen.current_level.auto_binding_zones.emplace_back(std::move(new_block));
			break;
		}
		default: assert("Logic Error");
	}
}

void Editor::set_camera_bound() noexcept {}

void Editor::delete_all_selected() noexcept {
	auto iter = [](auto& iter) {
		for (size_t i = iter.size() - 1; i + 1 > 0; --i) {
			if (((Editable*)&iter[i])->editor_selected) {
				iter.erase(BEG(iter) + i);
			}
		}
	};

	#define X(T, t) iter(game->play_screen.current_level. t);
	LIST_ENTITIES
	#undef X

#define X(T, t) game->play_screen.current_level. t .size() +
	auto n_elements = LIST_ENTITIES 0;
#undef X

	if (n_elements == 0) ask_to_save = true;
}

void Editor::save(const std::filesystem::path& path) noexcept {
	save_to_json_file((dyn_struct)game->play_screen.current_level, path);
	ask_to_save = false;
}

void Editor::update_selected_pos() noexcept {
	auto func = [&](auto& vec) noexcept {
		for (auto& x : vec) if (((Editable*)&x)->editor_selected) {
			pos_modifier = &((Physicable*)&x)->rec.pos;
			return true;
		}
		return false;
	};

	pos_modifier = nullptr;

	#define X(T, t)\
	if constexpr (std::is_base_of_v<Editable, T> && std::is_base_of_v<Physicable, T>) {\
		if (!pos_modifier) func(game->play_screen.current_level. t);\
	}
	LIST_ENTITIES
	#undef X

	if (!pos_modifier) return;

	auto pos = IM::pos_view_to_window(game->play_screen.current_level.camera, *pos_modifier);
	
	bool x_hover = Rectanglef{pos, {300, 30}}.in(
		IM::getMouseScreenPos() - IM::getMouseScreenDelta()
	);
	bool y_hover = Rectanglef{pos, {30, 300}}.in(
		IM::getMouseScreenPos() - IM::getMouseScreenDelta()
	);

	if (x_hover && IM::isMousePressed(Mouse::Left)) {
		pos_modifier->x += IM::getMouseDeltaInView(game->play_screen.current_level.camera).x;
	}
	if (y_hover && IM::isMousePressed(Mouse::Left)) {
		pos_modifier->y += IM::getMouseDeltaInView(game->play_screen.current_level.camera).y;
	}

}

