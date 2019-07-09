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
	bool run_level = true;

	while (window.isOpen()) {
		float dt = dtClock.restart().asSeconds();
		IM::update(window);
		ImGui::SFML::Update(window, sf::seconds(dt));

		run_editor |= IM::isKeyJustPressed(sf::Keyboard::E);
		run_level &= !run_editor;
		run_level |= IM::isKeyJustPressed(sf::Keyboard::Return);
		run_editor &= !run_level;

		window.clear();

		if (run_editor) {
			editor.input();
			editor.update(dt);
			editor.render(window);
		}
		if (run_level) {
			level.update(dt);
		}
		level.render(window);

		ImGui::SFML::Render(window);
		window.display();
	}
	return 0;
}