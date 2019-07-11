#include "Editor.hpp"

#include "imgui.h"
#include "Managers/InputsManager.hpp"
#include "Math/Vector.hpp"

#include "Level.hpp"

#include <stdio.h>

#include "Collision.hpp"

void Editor::render(sf::RenderTarget& target) noexcept {
	ImGui::Begin("Editor");
	defer{ ImGui::End(); };

	if (ImGui::Button("Creating an element")) {
		element_creating = true;
		require_dragging= true;
		if (!element_to_create) element_to_create = (Creating_Element)0;
	}

	ImGui::Separator();
	ImGui::Text("Selected");
	if (ImGui::Button("Delete (Suppr)")) delete_all_selected();


	placing_player |= ImGui::Button("Place player");
	element_creating &= !placing_player;
	require_dragging &= !placing_player;
	require_dragging &= element_creating;

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

		for (auto& b : level_to_edit->blocks) {
			if (shift) {
				b.editor_selected |= test(b, IM::getMousePosInView(level_to_edit->camera));
			}
			else {
				b.editor_selected = test(b, IM::getMousePosInView(level_to_edit->camera));
			}
		}
	}
	if (IM::isKeyJustReleased(sf::Keyboard::Delete)) delete_all_selected();
}

void Editor::end_drag(Vector2f start, Vector2f end) noexcept {
	if (!level_to_edit) return;

	if (element_to_create) {
		Block new_block;
		new_block.pos = start;
		new_block.size = end - start;

		if (new_block.size.x < 0) new_block.pos.x += new_block.size.x;
		if (new_block.size.y < 0) new_block.pos.y += new_block.size.y;
		new_block.size.x = std::abs(new_block.size.x);
		new_block.size.y = std::abs(new_block.size.y);

		level_to_edit->blocks.emplace_back(std::move(new_block));
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
	auto delete_in_iter = [](auto& iter) {
		for (size_t i = iter.size() - 1; i + 1 > 0; --i) {
			if (iter[i].editor_selected) {
				iter.erase(BEG(iter) + i);
			}
		}
	};

	delete_in_iter(level_to_edit->blocks);
}
