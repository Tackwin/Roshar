#pragma once

#include "Managers/AssetsManager.hpp"
#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"
#include <GL/glew.h>
#include <GL/wglew.h>

namespace render {

	struct Sprite_Info {
		Vector2f pos;
		Vector2f size;
		Vector2f origin;
		float rotation;
		Vector4d color;
		asset::Key texture;
		asset::Key shader;
	};

	struct View_Info {
		Rectanglef world_bounds;
	};

	struct Order {
		union {
			Sprite_Info sprite;
			View_Info view;
		};

		Order() noexcept {
			sprite = {};
			kind = Kind::Sprite;
		}

		enum class Kind {
			Sprite = 0,
			View_Push,
			View_Pop,
			Count
		} kind;
	};

	extern View_Info current_view;

	Order sprite(Vector2f pos, Vector2f size, asset::Key texture) noexcept;
	Order push_view(Vector2f pos, Vector2f size) noexcept;
	Order push_view(Rectanglef bounds) noexcept;
	Order pop_view() noexcept;

	void sprite(Sprite_Info info) noexcept;
}
