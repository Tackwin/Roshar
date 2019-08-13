#include "Editor.hpp"

#include "imgui.h"
#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"
#include "os/file.hpp"

#include "Level.hpp"

#include <cstdio>
#include <array>
#include <algorithm>

#include "Collision.hpp"

void Editor::render(sf::RenderTarget& target) noexcept {
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
		file::open_dir_async([&s = save_path](std::optional<std::filesystem::path> path) {
			std::lock_guard guard{ Main_Mutex };
			if (path) s = path->string();
		});
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (level_to_edit && ImGui::Button("Save")) {
		if (ask_to_save) ImGui::OpenPopup("Sure ?");
		else save(save_path);
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (level_to_edit && ImGui::Button("Load")) {
		file::OpenFileOpts opts;
		opts.allow_multiple = false;
		opts.ext_filters["all"] = { "*.json" };
		file::open_file_async([&s = save_path, l = level_to_edit](file::OpenFileResult result) {
			if (!result.succeded) return;
			auto opt_dyn = load_from_json_file(result.filepath);
			if (!opt_dyn) return;
			
			std::lock_guard guard{ Main_Mutex };
			
			Level new_level = (Level)* opt_dyn;
			new_level.save_path = result.filepath;

			*l = new_level;
			Level_Store.initial_level = std::move(new_level);

			s = result.filepath.string();
		}, opts);
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (level_to_edit && ImGui::Button("Reload")) {
		auto old_markers = std::move(level_to_edit->markers);
		auto opt_dyn = load_from_json_file(save_path);
		if (!opt_dyn) return;
		Level new_level = (Level)* opt_dyn;
		new_level.save_path = save_path;

		*level_to_edit = new_level;
		Level_Store.initial_level = std::move(new_level);
		level_to_edit->markers = std::move(old_markers);
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
		decor.rec = { level_to_edit->camera.getCenter(), texture.asset.getSize() };
		decor.rec.pos -= decor.rec.size / 2;
		decor.sprite.setTexture(texture.asset);
		level_to_edit->decor_sprites.push_back(decor);
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

	auto pred = [](auto& x) {return x.editor_selected; };
	auto n_selected = std::count_if(BEG_END(level_to_edit->prest_sources), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Prest sources");
		float x = { 0 };
		if (n_selected == 1) x = std::find_if(BEG_END(level_to_edit->prest_sources), pred)->prest;

		ImGui::Text("Prest");
		ImGui::SameLine();
		if (ImGui::InputFloat("Prest", &x))
			for (auto& y : level_to_edit->prest_sources) if (pred(y)) y.prest = x;
	}

	n_selected = std::count_if(BEG_END(level_to_edit->doors), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Doors");
		bool closed = { true };
		if (n_selected == 1) closed = std::find_if(BEG_END(level_to_edit->doors), pred)->closed;

		ImGui::Text("Closed");
		ImGui::SameLine();
		if (ImGui::Checkbox("Closed", &closed))
			for (auto& y : level_to_edit->doors) if (pred(y)) y.closed = closed;

		thread_local bool must_list{ false };
		ImGui::Checkbox(must_list ? "must list" : "mustnt_list", &must_list);

		if (ImGui::Button("Add Selection to list")) {
			for (auto& y : level_to_edit->doors) {
				if (!pred(y)) continue;
				auto& list = must_list ? y.must_triggered : y.mustnt_triggered;

				for (const auto& z : level_to_edit->trigger_zones) {
					if (pred(z)) list.push_back(z.id);
				}

				list.erase(std::unique(BEG_END(list)), END(list));
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove selection from list")) {
			for (auto& y : level_to_edit->doors) {
				if (!pred(y)) continue;
				auto& list = must_list ? y.must_triggered : y.mustnt_triggered;

				for (const auto& z : level_to_edit->trigger_zones) {
					if (!pred(z)) continue;
					if (auto it = std::find(BEG_END(list), z.id); it != END(list)) list.erase(it);
				}
			}
		}
	}

	n_selected = std::count_if(BEG_END(level_to_edit->next_zones), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Next zones");
		char next[512];
		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(level_to_edit->next_zones), pred);
			strcpy_s(next, 512, it->next_level.c_str());
		}

		ImGui::Text("Next");
		ImGui::SameLine();
		if (ImGui::InputText("Next", next, sizeof(next))) {
			for (auto& y : level_to_edit->next_zones) if (pred(y)) y.next_level = next;
		}
	}

	n_selected = std::count_if(BEG_END(level_to_edit->auto_binding_zones), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Auto bindings zones");

		float x;
		float y;

		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(level_to_edit->auto_binding_zones), pred);
			x = it->binding.x;
			y = it->binding.y;
		}

		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::PushItemWidth(0.7f);
		defer{ ImGui::PopItemWidth(); };
		if (ImGui::InputFloat("X", &x, 0.05f))
			for (auto& a : level_to_edit->auto_binding_zones) if (pred(a)) a.binding.x = x;
		if (ImGui::InputFloat("Y", &y, 0.05f))
			for (auto& a : level_to_edit->auto_binding_zones) if (pred(a)) a.binding.y = y;
	}

	n_selected = std::count_if(BEG_END(level_to_edit->rocks), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Rocks");
		float mass;
		float radius;

		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(level_to_edit->rocks), pred);
			mass = it->mass;
			radius = it->r;
		}

		ImGui::Text("Mass");
		ImGui::SameLine();
		if (ImGui::InputFloat("Mass", &mass)) {
			for (auto& y : level_to_edit->rocks) if (pred(y)) y.mass = mass;
		}
		ImGui::Text("Radius");
		ImGui::SameLine();
		if (ImGui::InputFloat("Radius", &radius)) {
			for (auto& y : level_to_edit->rocks) if (pred(y)) y.r = radius;
		}
	}

	n_selected = std::count_if(BEG_END(level_to_edit->decor_sprites), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("Texture");

		float opacity{ 1 };
		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(level_to_edit->decor_sprites), pred);
			opacity = it->opacity;
		}

		ImGui::Text("Opacity");
		ImGui::SameLine();
		if (ImGui::InputFloat("Opacity", &opacity))
			for (auto& x : level_to_edit->decor_sprites) if (pred(x)) x.opacity = opacity;
	}

	n_selected = std::count_if(BEG_END(level_to_edit->dispensers), pred);
	if (n_selected) {
		ImGui::Separator();
		ImGui::Text("dispensers");
		float r{ 0 };
		float speed{ 0 };
		float hz{ 0 };
		float offset_timer{ 0.f };

		if (n_selected == 1) {
			auto it = std::find_if(BEG_END(level_to_edit->dispensers), pred);
			r = it->proj_r;
			speed = it->proj_speed;
			hz = it->hz;
			offset_timer = it->offset_timer;
		}

		ImGui::Text("Radius");
		ImGui::SameLine();
		if (ImGui::InputFloat("Radius", &r))
			for (auto& y : level_to_edit->dispensers) if (pred(y)) y.proj_r = r;

		ImGui::Text("Speed");
		ImGui::SameLine();
		if (ImGui::InputFloat("Speed", &speed))
			for (auto& y : level_to_edit->dispensers) if (pred(y)) y.proj_speed = speed;

		ImGui::Text("Cadence");
		ImGui::SameLine();
		if (ImGui::InputFloat("Cadence", &hz))
			for (auto& y : level_to_edit->dispensers) if (pred(y)) {
				y.hz = hz;
				y.timer = 1.f / hz;
			}

		ImGui::Text("Offset");
		ImGui::SameLine();
		if (ImGui::InputFloat("Offset", &offset_timer))
			for (auto& y : level_to_edit->dispensers) if (pred(y)) {
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

		Rectangle rec{ start, end - start };
		sf::RectangleShape shape;
		shape.setPosition(rec.pos);
		shape.setSize(rec.size);
		shape.setFillColor(Vector4d{ 0.2, 0.5, 0.2, 0.5 });
		target.draw(shape);

		auto view = target.getView();
		defer{ target.setView(view); };
		target.setView(target.getDefaultView());
	}

	if (start_selection) {
		sf::RectangleShape shape;
		shape.setPosition(*start_selection);
		shape.setSize(get_mouse_pos() - *start_selection);
		shape.setFillColor(Vector4d{ 0, 1, 0, 0.5 });
		target.draw(shape);
	}

	for (const auto& d : level_to_edit->doors) {
		Vector2f start = d.rec.center();

		for (const auto& x : d.must_triggered) {
			auto it = std::find_if(
				BEG_END(level_to_edit->trigger_zones), [x](auto& z) { return z.id == x; }
			);
			if (it == END(level_to_edit->trigger_zones)) continue;

			Vector2f end = it->rec.center();

			Vector2f::renderLine(target, start, end, { 0.1, 1.0, 0.1, 1.0 });
		}

		for (const auto& x : d.mustnt_triggered) {
			auto it = std::find_if(
				BEG_END(level_to_edit->trigger_zones), [x](auto& z) { return z.id == x; }
			);
			if (it == END(level_to_edit->trigger_zones)) continue;

			Vector2f end = it->rec.center();

			Vector2f::renderLine(target, start, end, { 1.0, 0.1, 0.1, 1.0 });
		}
	}

	if (snap_grid) {
		auto& cam = level_to_edit->camera;
		std::vector<std::array<sf::Vertex, 2>> vertices;

		size_t n = (size_t)(std::abs(cam.getSize().x) / snap_grid);
		for (size_t x = 0; x <= n; ++x) {
			Vector2f A = (Vector2f)cam.getCenter() - (Vector2f)cam.getSize() / 2;
			Vector2f B = (Vector2f)cam.getCenter() - (Vector2f)cam.getSize() / 2;
			A = snap_grid * (Vector2i)(A / snap_grid);
			B = snap_grid * (Vector2i)(B / snap_grid);

			B.y = cam.getCenter().y + cam.getSize().y / 2;

			A.x += x * snap_grid;
			B.x += x * snap_grid;

			std::array<sf::Vertex, 2> v;
			v[0] = sf::Vertex(A, sf::Color(50, 50, 50, 50));
			v[1] = sf::Vertex(B, sf::Color(50, 50, 50, 50));
			vertices.push_back(v);
		}
		n = (size_t)(std::abs(cam.getSize().y) / snap_grid);
		for (size_t y = 0; y <= n; ++y) {
			Vector2f A = (Vector2f)cam.getCenter() - (Vector2f)cam.getSize() / 2;
			Vector2f B = (Vector2f)cam.getCenter() - (Vector2f)cam.getSize() / 2;
			A = snap_grid * (Vector2i)(A / snap_grid);
			B = snap_grid * (Vector2i)(B / snap_grid);

			B.x = cam.getCenter().x + cam.getSize().x / 2;

			A.y -= y * snap_grid;
			B.y -= y * snap_grid;

			std::array<sf::Vertex, 2> v;
			v[0] = sf::Vertex(A, sf::Color(50, 50, 50, 50));
			v[1] = sf::Vertex(B, sf::Color(50, 50, 50, 50));
			vertices.push_back(v);
		}

		for (const auto& x : vertices) target.draw(x.data(), 2, sf::Lines);
	}
}

void Editor::update(float dt) noexcept {
	sin_time += std::cosf(sin_time) * dt;
	if (!level_to_edit) return;
	if (!IM::isWindowFocused()) return;

	bool shift = IM::isKeyPressed(sf::Keyboard::LShift);
	auto ctrl = IM::isKeyPressed(sf::Keyboard::LControl);
	auto scale = IM::getLastScroll();
	if (shift) scale /= 10;

	if (!edit_texture) level_to_edit->camera.zoom(math::scale_zoom(-scale + 1));
	else for (auto& b : level_to_edit->decor_sprites) {
		if (b.editor_selected) {
			auto center = b.rec.pos + b.rec.size / 2;
			b.rec.size *= math::scale_zoom(-scale + 1);
			b.rec.setCenter(center);
		}
	}

	if (IM::isKeyJustPressed(sf::Keyboard::F12)) {
		set_camera_bound();
	}

	if (require_dragging) {
		if (IM::isMouseJustPressed(sf::Mouse::Left) && !pos_start_drag) {
			pos_start_drag = get_mouse_pos();
			drag_offset.clear();
			for (auto& b : level_to_edit->decor_sprites)
				if (b.editor_selected) drag_offset.push_back(get_mouse_pos() - b.rec.pos);
		}
		if (edit_texture && IM::isMousePressed(sf::Mouse::Left)) {
			size_t i = 0;
			for (auto& b : level_to_edit->decor_sprites) {
				if (b.editor_selected) {
					b.rec.pos = get_mouse_pos() - drag_offset[i++];
				}
			}
		}
		if (IM::isMouseJustReleased(sf::Mouse::Left) && pos_start_drag) {
			Vector2f pos_end_drag = get_mouse_pos();
			end_drag(*pos_start_drag, pos_end_drag);
			pos_start_drag = std::nullopt;
		}
	}
	if (element_creating && element_to_create && IM::isMouseJustPressed(sf::Mouse::Left)) {
		switch (*element_to_create) {
		case Creating_Element::Prest: {
			Prest_Source p;
			p.pos = get_mouse_pos();
			p.prest = 1;
			level_to_edit->prest_sources.push_back(p);
			break;
		}
		case Creating_Element::Rock: {
			Rock r;
			r.pos = get_mouse_pos();
			r.r = 0.1f;
			r.mass = 1;
			level_to_edit->rocks.push_back(r);
			break;
		}
		default: break;
		}
	}

	if (IM::isMousePressed(sf::Mouse::Middle) && !IM::isMouseJustPressed(sf::Mouse::Middle)) {
		level_to_edit->camera.move(-1 * IM::getMouseDeltaInView(level_to_edit->camera));
	}
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		if (placing_player) {
			placing_player = false;

			level_to_edit->player.pos = get_mouse_pos();
			level_to_edit->player.velocity = {};
		}
		if (edit_texture) {
			require_dragging = true;
		}
	}
	if (IM::isMouseJustPressed(sf::Mouse::Right)) {
		start_selection = get_mouse_pos();
	}
	else if (start_selection && IM::isMouseJustReleased(sf::Mouse::Right)) {
		defer{ start_selection.reset(); };
		Rectanglef rec{*start_selection, get_mouse_pos() - *start_selection};

		auto iter = [cam = level_to_edit->camera, shift, ctrl, rec](auto& c) noexcept {
			for (auto& b : c) {
				bool orig = (shift && b.editor_selected);

				if (ctrl)	b.editor_selected &= !test(b, rec);
				else		b.editor_selected = orig || test(b, rec);
			}
		};

		iter(level_to_edit->rocks);
		iter(level_to_edit->doors);
		iter(level_to_edit->blocks);
		iter(level_to_edit->dry_zones);
		iter(level_to_edit->kill_zones);
		iter(level_to_edit->next_zones);
		iter(level_to_edit->dispensers);
		iter(level_to_edit->trigger_zones);
		iter(level_to_edit->prest_sources);
		iter(level_to_edit->auto_binding_zones);

		drag_offset.clear();
		if (edit_texture) {
			for (auto& b : level_to_edit->decor_sprites) {
				b.editor_selected = b.rec.intersect(rec);
			}
		}
		else {
			for (auto& b : level_to_edit->decor_sprites) b.editor_selected = false;
		}
	}
	if (IM::isKeyJustReleased(sf::Keyboard::Delete)) delete_all_selected();
	snap_vertical = IM::isKeyPressed(sf::Keyboard::LShift) && IM::isKeyPressed(sf::Keyboard::V);
	snap_horizontal = IM::isKeyPressed(sf::Keyboard::LShift) && IM::isKeyPressed(sf::Keyboard::H);
}

Vector2f Editor::get_mouse_pos() const noexcept {
	auto pos = IM::getMousePosInView(level_to_edit->camera);;
	if (snap_grid == 0) return pos;

	return snap_grid * (Vector2i)(pos / snap_grid);
}

void Editor::end_drag(Vector2f start, Vector2f end) noexcept {
	if (!level_to_edit) return;

	if (element_to_create){
		switch (*element_to_create) {
		case Creating_Element::Block: {
			Block new_block;
			new_block.pos = start;
			new_block.size = end - start;

			if (new_block.size.x < 0) new_block.pos.x += new_block.size.x;
			if (new_block.size.y < 0) new_block.pos.y += new_block.size.y;
			new_block.size.x = std::abs(new_block.size.x);
			new_block.size.y = std::abs(new_block.size.y);

			level_to_edit->blocks.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Kill_Zone: {
			Kill_Zone new_block;
			new_block.pos = start;
			new_block.size = end - start;

			if (new_block.size.x < 0) new_block.pos.x += new_block.size.x;
			if (new_block.size.y < 0) new_block.pos.y += new_block.size.y;
			new_block.size.x = std::abs(new_block.size.x);
			new_block.size.y = std::abs(new_block.size.y);

			level_to_edit->kill_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Next_Zone: {
			Next_Zone new_block;
			new_block.pos = start;
			new_block.size = end - start;

			if (new_block.size.x < 0) new_block.pos.x += new_block.size.x;
			if (new_block.size.y < 0) new_block.pos.y += new_block.size.y;
			new_block.size.x = std::abs(new_block.size.x);
			new_block.size.y = std::abs(new_block.size.y);

			level_to_edit->next_zones.emplace_back(std::move(new_block));
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

			level_to_edit->dispensers.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Dry_Zone: {
			Dry_Zone new_block;
			new_block.rec = { start, end - start };

			level_to_edit->dry_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Trigger_Zone: {
			Trigger_Zone new_block;
			new_block.rec = { start, end - start };
			new_block.id = xstd::uuid();

			level_to_edit->trigger_zones.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Door: {
			Door new_block;
			new_block.rec = { start, end - start };

			level_to_edit->doors.emplace_back(std::move(new_block));
			break;
		}
		case Creating_Element::Auto_Binding_Zone: {
			Auto_Binding_Zone new_block;
			new_block.rec = { start, end - start };
			new_block.uuid = xstd::uuid();

			level_to_edit->auto_binding_zones.emplace_back(std::move(new_block));
			break;
		}
		}
	}
}

void Editor::set_camera_bound() noexcept {
	auto& cam = level_to_edit->camera;
	Vector2f center = cam.getCenter();
	Vector2f size = cam.getSize();
	level_to_edit->camera_bound = { center - size / 2, size };
}

void Editor::delete_all_selected() noexcept {
	auto iter = [](auto& iter) {
		for (size_t i = iter.size() - 1; i + 1 > 0; --i) {
			if (iter[i].editor_selected) {
				iter.erase(BEG(iter) + i);
			}
		}
	};

	iter(level_to_edit->rocks);
	iter(level_to_edit->doors);
	iter(level_to_edit->blocks);
	iter(level_to_edit->dry_zones);
	iter(level_to_edit->kill_zones);
	iter(level_to_edit->next_zones);
	iter(level_to_edit->dispensers);
	iter(level_to_edit->trigger_zones);
	iter(level_to_edit->prest_sources);
	iter(level_to_edit->decor_sprites);
	iter(level_to_edit->auto_binding_zones);

#define S(x) level_to_edit->x.size()
	auto n_elements =
		S(rocks) +
		S(doors) +
		S(blocks) +
		S(dry_zones) +
		S(kill_zones) +
		S(next_zones) +
		S(trigger_zones) +
		S(prest_sources) +
		S(decor_sprites) +
		S(auto_binding_zones) +
		S(dispensers);
#undef S

	if (n_elements == 0) ask_to_save = true;
}

void Editor::save(const std::filesystem::path& path) noexcept {
	save_to_json_file((dyn_struct)* level_to_edit, path);
	ask_to_save = false;
}

