#pragma once

#include <string>
#include <optional>
#include "Math/Vector.hpp"
#include "Profil/Profile.hpp"
#include "Graphic/Graphics.hpp"


namespace kit {
	struct State {
		struct Button_State {
			bool hovering{ false };
			float time_hovered{ 0.f };
		};
		struct Key_Prompt_State {
			bool hovering{ false };
			bool focused{ false };
		};
		struct Card_State {
			inline static size_t Card_Id = 0;
			bool hovering{ false };
			bool selected{ false };
			float time_hovered{ 0.f };
			std::string name;
		};
		struct Input_Text_State {
			float hovering{ false };
			float time_hovered{ 0.f };
			std::string str;
			size_t cursor_pos{0};
		};
		std::unordered_map<std::string, Button_State> button_states;
		std::unordered_map<std::string, Key_Prompt_State> key_prompt_sates;
		std::unordered_map<size_t, Card_State> card_states;
		std::unordered_map<std::string, Input_Text_State> input_text_states;

		Vector2f current_pos;
		Vector2f current_origin{ 0, 0 };

		IM::Input_Iterator last_input{ IM::end() };

		float button_margin{ 5.f };

		float text_margin{2.f};

		float card_padding{ 4.f };
		float card_margin{ 5.f };
		float card_title_size{ 28.f };
		float card_uptime_size{ 12.f };
		Vector2f card_min_size{ 300, 475 };
		Vector4d card_color{ 0.129, 0.161, 0.227, 1.0 };

		render::Orders orders;
	};

	extern void input(IM::Input_Iterator it) noexcept;
	extern void update(float dt) noexcept;
	extern void render(render::Orders& target) noexcept;
	extern State& get_state() noexcept;

	extern void key_prompt(std::string label, float size, Keyboard::Key& binded) noexcept;
	extern void key_prompt(
		std::string label, float size, Control_Bindings::Action& binded
	) noexcept;
	extern bool label(std::string text) noexcept;
	extern bool button(std::string label, float size) noexcept;
	extern bool card(Profile& profile) noexcept;
	extern std::optional<State::Card_State> card_plus() noexcept;
	extern std::string text_input(std::string def, float size) noexcept;
};