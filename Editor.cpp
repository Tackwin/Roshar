#include "Editor.hpp"

#include "imgui.h"
#include "Managers/InputsManager.hpp"

#include "Level.hpp"

void Editor::input() noexcept {
	if (!level_to_edit) return;

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
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		if (placing_player) {
			placing_player = false;

			level_to_edit->player.pos = IM::getMousePosInView(level_to_edit->camera);
			level_to_edit->player.velocity = {};
		}
	}
}

void Editor::render(sf::RenderTarget& target) noexcept {
	ImGui::Begin("Editor");
	ImGui::Text("%s", element_to_create ? "Creating" : "Creating an element");
	if (ImGui::IsItemClicked()) {
		if (element_to_create) element_to_create.reset();
		else element_to_create = (Creating_Element)0;
		require_dragging = !require_dragging;
	}

	placing_player |= ImGui::Button("Place player");
	require_dragging &= !placing_player;

	if (element_to_create) {
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
	ImGui::End();
}

void Editor::update(float dt) noexcept {
	if (!level_to_edit) return;

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
