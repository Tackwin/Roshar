#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <optional>

#include "Math/Vector.hpp"

#include "Managers/InputsManager.hpp"

#include "imgui-SFML.h"
#include "imgui.h"

#include "Editor.hpp"
#include "Level.hpp"

int main(int, char**) {
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Roshar");

	ImGui::SFML::Init(window);
	defer{ ImGui::SFML::Shutdown(); };

	sf::Clock dtClock;

	Level level;
	Editor editor;
	editor.level_to_edit = &level;

	bool run_editor = false;

	while (window.isOpen()) {
		float dt = dtClock.restart().asSeconds();
		IM::update(window);
		ImGui::SFML::Update(window, sf::seconds(dt));

		if (IM::isKeyJustPressed(sf::Keyboard::E)) {
			run_editor = !run_editor;
		}

		window.clear();

		ImGui::Begin("Environment");
		ImGui::InputInt("Drag angle step", &Environment.drag_angle_step);
		ImGui::InputFloat("Drag", &Environment.drag);
		ImGui::InputFloat("Gravity", &Environment.gravity);
		ImGui::End();

		if (run_editor) {
			editor.update(dt);
			editor.render(window);
		}
		else {
			level.update(dt);
		}
		level.render(window);

		ImGui::SFML::Render(window);
		window.display();
	}
	return 0;
}