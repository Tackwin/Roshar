#include "Editor.hpp"

#include "imgui.h"
#include "Managers/InputsManager.hpp"
#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"
#include "os/file.hpp"

#include "Level.hpp"

#include <cstdio>
#include <algorithm>

#include "Collision.hpp"

void Editor::render(sf::RenderTarget& target) noexcept {
	ImGui::Begin("Editor");
	defer{ ImGui::End(); };

	if (ImGui::Button("Creating an element")) {
		element_creating = true;
		require_dragging = true;
		if (!element_to_create) element_to_create = (Creating_Element)0;
	}

	ImGui::Separator();

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (ImGui::Button("Open")) {
		file::open_dir_async([&s = save_path](std::optional<std::filesystem::path> path) {
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
			*l = (Level)* opt_dyn;
			s = result.filepath.string();
		}, opts);
	}
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (level_to_edit && ImGui::Button("Reload")) {
		auto old_markers = std::move(level_to_edit->markers);
		auto opt_dyn = load_from_json_file(save_path);
		if (!opt_dyn) return;
		*level_to_edit = (Level)* opt_dyn;
		level_to_edit->markers = std::move(old_markers);
	}
	char buffer[512];
	strcpy_s(buffer, save_path.data());
	ImGui::PushItemWidth(ImGui::GetWindowWidth());
	ImGui::InputText("", buffer, 512);
	save_path = buffer;
	ImGui::Separator();
	ImGui::Text("Selected");
	if (ImGui::Button("Delete (Suppr)")) delete_all_selected();


	placing_player |= ImGui::Button("Place player");
	element_creating &= !placing_player;
	require_dragging &= !placing_player;
	require_dragging &= element_creating;

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
		if (ImGui::InputFloat("Offset", &offset_timer)) for (auto& y : level_to_edit->dispensers)
				if (pred(y)) y.offset_timer = offset_timer;
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

	if (pos_start_drag) {
		auto view = target.getView();
		defer{ target.setView(view); };
		target.setView(target.getDefaultView());

		if (snap_horizontal) {
			auto pos = Vector2f{ pos_start_drag->x, (float)IM::getMouseScreenPos().y };
			sf::CircleShape mark(3);
			mark.setFillColor(Vector4f{ 1.f, 1.f, 1.f, 0.5f });
			mark.setPosition(pos);
			target.draw(mark);
			Vector2f::renderLine(target, *pos_start_drag, pos, { 1.0, 1.0, 1.0, 0.2 });
		}
		if (snap_vertical) {
			auto pos = Vector2f{ (float)IM::getMouseScreenPos().x, pos_start_drag->y };
			sf::CircleShape mark(3);
			mark.setFillColor(Vector4f{ 1.f, 1.f, 1.f, 0.5f });
			mark.setPosition(pos);
			target.draw(mark);
			Vector2f::renderLine(target, *pos_start_drag, pos, { 1.0, 1.0, 1.0, 0.2 });
		}
	}

	if (start_selection) {
		sf::RectangleShape shape;
		shape.setPosition(*start_selection);
		shape.setSize(IM::getMousePosInView(level_to_edit->camera) - *start_selection);
		shape.setFillColor(Vector4d{ 0, 1, 0, 0.5 });
		target.draw(shape);
	}
}

void Editor::update(float dt) noexcept {
	sin_time += std::cosf(sin_time) * dt;
	if (!level_to_edit) return;
	if (!IM::isWindowFocused()) return;

	level_to_edit->camera.zoom(math::scale_zoom(-IM::getLastScroll() + 1));

	if (require_dragging) {
		if (IM::isMouseJustPressed(sf::Mouse::Left) && !pos_start_drag) {
			pos_start_drag = IM::getMousePosInView(level_to_edit->camera);
		}
		if (IM::isMouseJustReleased(sf::Mouse::Left) && pos_start_drag) {
			Vector2f pos_end_drag = IM::getMousePosInView(level_to_edit->camera);
			end_drag(*pos_start_drag, pos_end_drag);
			pos_start_drag = std::nullopt;
		}
	}
	if (element_creating && element_to_create && IM::isMouseJustPressed(sf::Mouse::Left)) {
		switch (*element_to_create) {
		case Creating_Element::Prest: {
			Prest_Source p;
			p.pos = IM::getMousePosInView(level_to_edit->camera);
			p.prest = 1;
			level_to_edit->prest_sources.push_back(p);
			break;
		}
		case Creating_Element::Rock: {
			Rock r;
			r.pos = IM::getMousePosInView(level_to_edit->camera);
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

			level_to_edit->player.pos = IM::getMousePosInView(level_to_edit->camera);
			level_to_edit->player.velocity = {};
		}
	}
	if (IM::isMouseJustPressed(sf::Mouse::Right)) {
		start_selection = IM::getMousePosInView(level_to_edit->camera);
	}
	else if (start_selection && IM::isMouseJustReleased(sf::Mouse::Right)) {
		defer{ start_selection.reset(); };
		Rectanglef rec{
			*start_selection,
			IM::getMousePosInView(level_to_edit->camera) - *start_selection
		};

		auto shift = IM::isKeyPressed(sf::Keyboard::LShift);

		auto iter = [cam = level_to_edit->camera, shift, rec](auto& c) noexcept {
			for (auto& b : c) {
				bool oring = (shift && b.editor_selected);
				b.editor_selected = oring || test(b, rec);
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
	}
	if (IM::isKeyJustReleased(sf::Keyboard::Delete)) delete_all_selected();
	snap_vertical = IM::isKeyPressed(sf::Keyboard::LShift) && IM::isKeyPressed(sf::Keyboard::V);
	snap_horizontal = IM::isKeyPressed(sf::Keyboard::LShift) && IM::isKeyPressed(sf::Keyboard::H);
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
		}
	}
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
		S(dispensers);
#undef S

	if (n_elements == 0) ask_to_save = true;
}

void Editor::save(const std::filesystem::path& path) noexcept {
	save_to_json_file((dyn_struct)* level_to_edit, path);
	ask_to_save = false;
}

