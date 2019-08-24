#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <optional>

#include "Math/Vector.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"

#include "imgui-SFML.h"
#include "imgui.h"

#include "Editor.hpp"
#include "Game.hpp"

int main(int, char**) {
	Environment.window_width = 1280;
	Environment.window_height = 720;
	sf::RenderWindow render_window(sf::VideoMode(Environment.window_width,Environment.window_height), "Roshar");
	window = &render_window;

	defer{ asset::Store.textures.clear(); };

	ImGui::SFML::Init(render_window);
	defer{ ImGui::SFML::Shutdown(); };

	asset::Store.monitor_path("textures/");
	asset::Store.load_known_textures();

	float max_dt = 0;
	size_t last_dt_count = 60;
	std::vector<float> last_dt;

	game = new Game;
	defer{ delete game; };

	sf::Clock dtClock;
	while (render_window.isOpen()) {
		float dt = dtClock.restart().asSeconds();
		max_dt = std::max(dt, max_dt);
		last_dt.insert(BEG(last_dt), dt);
		last_dt.resize(last_dt_count);

		sf::Event event;
		while (render_window.pollEvent(event)) {

			ImGui::SFML::ProcessEvent(event);

			if (!render_window.hasFocus()) continue;

			if (event.type == sf::Event::Closed)
				render_window.close();

			if (event.type == sf::Event::MouseWheelScrolled) {
				wheel_scroll = event.mouseWheelScroll.delta;
			}
		}

		ImGui::SFML::Update(render_window, sf::seconds(dt));

		Main_Mutex.lock();
		defer{ Main_Mutex.unlock(); };

		render_window.clear({ 153, 78, 102, 255 });

		ImGui::Begin("Environment");
		ImGui::InputInt("Drag angle step", &Environment.drag_angle_step);
		ImGui::InputFloat("Gather speed", &Environment.gather_speed);
		ImGui::InputFloat("Gather step", &Environment.gather_step);
		ImGui::InputFloat("Drag", &Environment.drag);
		ImGui::InputFloat("Binding range", &Environment.binding_range);
		ImGui::InputFloat("Gravity", &Environment.gravity);
		ImGui::InputFloat("Dead velocity", &Environment.dead_velocity);
		ImGui::InputFloat("Speed up", &Environment.speed_up_step);
		ImGui::End();

		ImGui::Begin("Perf");

		float avg = 0;
		for (auto x : last_dt) avg += x;
		avg /= last_dt_count;

		ImGui::Text(
			"current dt: %4.2f ms, avg(%u): %4.2f ms, max: %4.2f",
			1000 * dt,
			last_dt_count,
			1000 * avg,
			1000 * max_dt
		);
		ImGui::Text("Fps: %d", (size_t)(1 / dt));

		ImGui::End();

		game->update((size_t)(1'000'000 * dt));
		game->render(render_window);

		ImGui::SFML::Render(render_window);
		render_window.display();
	}
	return 0;
}
