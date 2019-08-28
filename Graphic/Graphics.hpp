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

	struct Circle_Info {
		float r;
		Vector2f pos;
		float outline;
		Vector4d color;
		Vector4d outline_color;
	};

	struct Rectangle_Info {
		Vector2f size;
		Vector2f pos;
		float outline;
		Vector4d color;
		Vector4d outline_color;
	};

	struct View_Info {
		Rectanglef world_bounds;
	};

	struct Order {
		union {
			Rectangle_Info rectangle;
			Circle_Info circle;
			Sprite_Info sprite;
			View_Info view;
		};

		Order() noexcept {
			sprite = {};
			kind = Kind::Sprite;
		}

		enum class Kind {
			Sprite = 0,
			Rectangle,
			View_Push,
			View_Pop,
			Circle,
			Count
		} kind;
	};

	extern View_Info current_view;

	Order sprite(
		Vector2f pos,
		Vector2f size,
		asset::Key texture,
		Vector2f origin = { 0, 0 },
		float rotation = 0.f,
		Vector4d color = {1, 1, 1, 1},
		asset::Key shader = 0
	) noexcept;
	Order circle(
		float r,
		Vector2f pos,
		Vector4d color,
		float outline = 0.f,
		Vector4d outline_color = { 0., 0., 0., 0. }
	) noexcept;
	Order rectangle(
		Vector2f pos,
		Vector2f size,
		Vector4d color,
		float outline = 0.f,
		Vector4d outline_color = { 0, 0, 0, 0 }
	) noexcept;
	Order push_view(Vector2f pos, Vector2f size) noexcept;
	Order push_view(Rectanglef bounds) noexcept;
	Order pop_view() noexcept;

	void immediate(Sprite_Info    info) noexcept;
	void immediate(Circle_Info    info) noexcept;
	void immediate(Rectangle_Info info) noexcept;
}
