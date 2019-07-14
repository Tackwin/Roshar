#include "Editor.hpp"

#include "imgui.h"
#include "Managers/InputsManager.hpp"
#include "Math/Vector.hpp"
#include "os/file.hpp"

#include "Level.hpp"

#include <stdio.h>
#include <algorithm>

#include "Collision.hpp"

void Editor::render(sf::RenderTarget&) noexcept {
	ImGui::Begin("Editor");
	defer{ ImGui::End(); };

	if (ImGui::Button("Creating an element")) {
		element_creating = true;
		require_dragging= true;
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
		std::wstring path;
		std::copy(BEG_END(save_path), std::back_inserter(path));
		save_to_json_file((dyn_struct)*level_to_edit, std::filesystem::path{ path });
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
	}ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	if (level_to_edit && ImGui::Button("Reload")) {
		auto opt_dyn = load_from_json_file(save_path);
		if (!opt_dyn) return;
		*level_to_edit = (Level)* opt_dyn;
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
	auto prest_selected = std::count_if(BEG_END(level_to_edit->prest_sources), pred);
	if (prest_selected) {
		float x = { 0 };
		if (prest_selected == 1)
			x = std::find_if(BEG_END(level_to_edit->prest_sources), pred)->prest;

		if (ImGui::InputFloat("Prest", &x))
			for (auto& y : level_to_edit->prest_sources) if (pred(y)) y.prest = x;
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
	if (element_creating && *element_to_create == Creating_Element::Prest) {
		if (IM::isMouseJustPressed(sf::Mouse::Left)) {
			Prest_Source p;
			p.pos = IM::getMousePosInView(level_to_edit->camera);
			p.prest = 1;
			level_to_edit->prest_sources.push_back(p);
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
		auto shift = IM::isKeyPressed(sf::Keyboard::LShift);

		auto iter = [cam = level_to_edit->camera, shift](auto& c) noexcept {
			for (auto& b : c) {
				bool oring = (shift && b.editor_selected);
				b.editor_selected = oring || test(b, IM::getMousePosInView(cam));
			}
		};

		iter(level_to_edit->blocks);
		iter(level_to_edit->kill_zones);
		iter(level_to_edit->prest_sources);
	}
	if (IM::isKeyJustReleased(sf::Keyboard::Delete)) delete_all_selected();
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
		}
	}
}

void Editor::render_selected(sf::RenderTarget& target, Block& block) noexcept {
	sf::RectangleShape shape(block.size + Vector2f{2, 2});
	shape.setOutlineColor(Vector4f{ 1.0, 1.0, 1.0, 1.0 });
	shape.setOutlineThickness(1);
	shape.setFillColor(Vector4f{ 1.0, 1.0, 1.0, sin_time });
	shape.setPosition(block.pos - Vector2f{ 1, 1 });
	target.draw(shape);
}

void Editor::delete_all_selected() noexcept {
	auto delete_in_iterable = [](auto& iter) {
		for (size_t i = iter.size() - 1; i + 1 > 0; --i) {
			if (iter[i].editor_selected) {
				iter.erase(BEG(iter) + i);
			}
		}
	};

	delete_in_iterable(level_to_edit->blocks);
	delete_in_iterable(level_to_edit->kill_zones);
	delete_in_iterable(level_to_edit->prest_sources);
}
