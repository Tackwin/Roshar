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
	Environment.window_width = 1280;
	Environment.window_height = 720;
	sf::RenderWindow window(
		sf::VideoMode(Environment.window_width, Environment.window_height), "Roshar"
	);

	ImGui::SFML::Init(window);
	defer{ ImGui::SFML::Shutdown(); };

	sf::Clock dtClock;

	Level level;
	Editor editor;
	editor.level_to_edit = &level;

	bool run_editor = false;

	while (window.isOpen()) {
		float dt = dtClock.restart().asSeconds();
		IM::update(window, dt);

		if (IM::isKeyJustPressed(sf::Keyboard::E)) {
			run_editor = !run_editor;
			if (!run_editor) {
				level.resume();
			}
			else {
				level.pause();
			}
		}

		window.clear({ 153, 78, 102, 255 });

		ImGui::Begin("Environment");
		ImGui::InputInt("Drag angle step", &Environment.drag_angle_step);
		ImGui::InputFloat("Gather speed", &Environment.gather_speed);
		ImGui::InputFloat("Gather step", &Environment.gather_step);
		ImGui::InputFloat("Drag", &Environment.drag);
		ImGui::InputFloat("Binding range", &Environment.binding_range);
		ImGui::InputFloat("Gravity", &Environment.gravity);
		ImGui::InputFloat("Dead velocity", &Environment.dead_velocity);
		ImGui::End();

		level.render(window);
		if (run_editor) {
			editor.update(dt);
			editor.render(window);
		}
		else {
			level.update(dt);
		}

		ImGui::SFML::Render(window);
		window.display();
	}
	return 0;
}