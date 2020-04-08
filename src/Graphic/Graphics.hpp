#pragma once

#include <vector>

#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"
#include "Assets.hpp"

namespace render {

	struct Arrow_Info {
		Vector2f a;
		Vector2f b;
		Vector4d color;
	};

	struct Line_Info {
		Vector2f a;
		Vector2f b;
		Vector4d color;
		float thickness;
	};

	struct Sprite_Info {
		Vector2f pos;
		Vector2f size;
		Vector2f origin;
		float rotation;
		Vector4d color;
		Rectanglef texture_rect;
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
		Rectanglef screen_bounds;
	};

	struct Point_Light_Info {
		Vector2f pos;
		Vector4d color;
		float intensity;
		float angle;
		float spread;
		size_t idx;
	};

	struct Ambient_Light {
		Vector4d color;
		float intensity;
	};

	struct Text_Info {
		asset::Key font_id;
		char* text;
		size_t text_size;
		float height;
		Vector2f origin;
		Vector2f pos;
		Vector4d color;

		enum Style {
			Normal = 0,
			Underline = 1,
			Count = 2
		};

		std::uint32_t style_mask;
	};

	struct Order {
		union {
			Point_Light_Info point_light;
			Ambient_Light ambient_light;
			Rectangle_Info rectangle;
			Circle_Info circle;
			Sprite_Info sprite;
			Arrow_Info arrow;
			Line_Info line;
			View_Info view;
			Text_Info text;
		};

		Order() noexcept {
			sprite = {};
			kind = Kind::Sprite;
		}

		enum class Kind {
			Sprite = 0,
			Ambient_Light_Push,
			Ambient_Light_Pop,
			Point_Light,
			Rectangle,
			View_Push,
			View_Pop,
			Circle,
			Arrow,
			Line,
			Text,
			Count
		} kind;
	};

	Order text(
		Vector2f pos,
		asset::Key font_id,
		char* text,
		size_t text_size,
		float size,
		Vector2f origin={0, 0},
		std::uint32_t style = Text_Info::Style::Normal,
		Vector4d color = {1, 1, 1, 1}
	) noexcept;
	Order arrow(
		Vector2f a,
		Vector2f b,
		Vector4d color
	) noexcept;
	Order line(
		Vector2f a,
		Vector2f b,
		Vector4d color,
		float thickness
	) noexcept;
	Order sprite(
		Vector2f pos,
		Vector2f size,
		asset::Key texture,
		Rectanglef texture_rect = { {0, 0}, {1, 1} },
		Vector2f origin = { 0, 0 },
		float rotation = 0.f,
		Vector4d color = { 1, 1, 1, 1 },
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
	Order point_light(
		Vector2f pos,
		Vector4d color,
		float intensity,
		float angle,
		float spread
	) noexcept;

	Order push_view(Vector2f pos, Vector2f size) noexcept;
	Order push_view(Rectanglef bounds) noexcept;
	Order pop_view() noexcept;

	struct Orders {
		std::vector<Order> objects;
		std::vector<Order> late;
		std::vector<Order> lights;

		std::vector<std::uint8_t> data;

		void clear() noexcept;

		void push_ambient_light(Vector4d color, float intensity) noexcept;
		void pop_ambient_light() noexcept;

		void append(const Orders& orders) noexcept {
			objects.insert(std::end(objects), BEG_END(orders.objects));
			late.insert(std::end(late), BEG_END(orders.late));
			lights.insert(std::end(lights), BEG_END(orders.lights));
			data.insert(std::end(data), BEG_END(orders.data));
		}

		void push_arrow(Vector2f a, Vector2f b, Vector4d color) noexcept {
			objects.emplace_back(arrow(a, b, color));
		}
		void push_line(Vector2f a, Vector2f b, Vector4d color, float thickness) noexcept {
			objects.emplace_back(line(a, b, color, thickness));
		}
		void late_push_line(Vector2f a, Vector2f b, Vector4d color, float thickness) noexcept {
			late.emplace_back(line(a, b, color, thickness));
		}

		void push_point_light(
			Vector2f pos,
			Vector4d color,
			float intensity,
			float angle = 0.f,
			float spread = 3.1415926f * 2.f
		) noexcept {
			auto info = point_light(pos, color, intensity, angle, spread);
			info.point_light.idx = lights.size();
			lights.emplace_back(std::move(info));
		}

		void push_text(
			Vector2f pos,
			asset::Key font_id,
			std::string str_text,
			float height,
			Vector2f origin,
			std::uint32_t style = Text_Info::Style::Normal,
			Vector4d color = {1, 1, 1, 1}
		) noexcept {
			size_t idx = data.size();
			for (auto& c : str_text) {
				data.push_back(c);
			}

			objects.emplace_back(text(
				pos, font_id, (char*)idx, str_text.size(), height, origin, style, color
			));
		}

		void push_rectangle(
			Vector2f pos,
			Vector2f size,
			Vector4d color,
			float outline = 0.f,
			Vector4d outline_color = { 0, 0, 0, 0 }
		) noexcept {
			objects.emplace_back(rectangle(pos, size, color, outline, outline_color));
		}
		void push_rec(
			Rectanglef rec,
			Vector4d color,
			float outline = 0.f,
			Vector4d outline_color = { 0, 0, 0, 0 }
		) noexcept {
			objects.emplace_back(rectangle(rec.pos, rec.size, color, outline, outline_color));
		}

		void late_push_text(
			Vector2f pos,
			asset::Key font_id,
			std::string str_text,
			float height,
			Vector2f origin,
			std::uint32_t style = Text_Info::Style::Normal,
			Vector4d color = {1, 1, 1, 1}
		) noexcept {
			size_t idx = data.size();
			for (auto& c : str_text) {
				data.push_back(c);
			}

			late.emplace_back(text(
				pos, font_id, (char*)idx, str_text.size(), height, origin, style, color
			));
		}


		void late_push_rectangle(
			Vector2f pos,
			Vector2f size,
			Vector4d color,
			float outline = 0.f,
			Vector4d outline_color = { 0, 0, 0, 0 }
		) noexcept {
			late.emplace_back(rectangle(pos, size, color, outline, outline_color));
		}

		void late_push_rec(
			Rectanglef rec,
			Vector4d color,
			float outline = 0.f,
			Vector4d outline_color = { 0, 0, 0, 0 }
		) noexcept {
			late.emplace_back(rectangle(rec.pos, rec.size, color, outline, outline_color));
		}

		void late_push_sprite(
			Rectanglef rec,
			asset::Key texture,
			Rectanglef texture_rect = { {0, 0}, {1, 1} },
			Vector2f origin = { 0, 0 },
			float rotation = 0.f,
			Vector4d color = { 1, 1, 1, 1 },
			asset::Key shader = 0
		) noexcept {
			late.emplace_back(sprite(
				rec.pos, rec.size, texture, texture_rect, origin, rotation, color, shader
			));
		}

		void push_rectangle(
			Rectanglef rec,
			Vector4d color,
			float outline = 0.f,
			Vector4d outline_color = { 0, 0, 0, 0 }
		) noexcept {
			objects.emplace_back(rectangle(rec.pos, rec.size, color, outline, outline_color));
		}

		void push_circle(
			float r,
			Vector2f pos,
			Vector4d color,
			float outline = 0.f,
			Vector4d outline_color = { 0., 0., 0., 0. }
		) noexcept {
			objects.emplace_back(circle(r, pos, color, outline, outline_color));
		}
		void late_push_circle(
			float r,
			Vector2f pos,
			Vector4d color,
			float outline = 0.f,
			Vector4d outline_color = { 0., 0., 0., 0. }
		) noexcept {
			late.emplace_back(circle(r, pos, color, outline, outline_color));
		}

		void push_sprite(
			Vector2f pos,
			Vector2f size,
			asset::Key texture,
			Rectanglef texture_rect = { {0, 0}, {1, 1} },
			Vector2f origin = { 0, 0 },
			float rotation = 0.f,
			Vector4d color = { 1, 1, 1, 1 },
			asset::Key shader = 0
		) noexcept;

		void push_sprite(
			Rectanglef rec,
			asset::Key texture,
			Rectanglef texture_rect = { {0, 0}, {1, 1} },
			Vector2f origin = { 0, 0 },
			float rotation = 0.f,
			Vector4d color = { 1, 1, 1, 1 },
			asset::Key shader = 0
		) noexcept;

		void push_view(Vector2f pos, Vector2f size) noexcept {
			push_view({ pos, size });
		}
		void push_view(Rectanglef bounds) noexcept {
			objects.emplace_back(render::push_view(bounds));
		}
		void pop_view() noexcept {
			objects.emplace_back(render::pop_view());
		}

		void late_push_view(Vector2f pos, Vector2f size) noexcept {
			late_push_view({ pos, size });
		}
		void late_push_view(Rectanglef bounds) noexcept {
			late.emplace_back(render::push_view(bounds));
		}
		void late_pop_view() noexcept {
			late.emplace_back(render::pop_view());
		}
	};

	extern View_Info current_view;

	void immediate(Text_Info           info) noexcept;
	void immediate(Point_Light_Info    info) noexcept;
	void immediate(Sprite_Info         info) noexcept;
	void immediate(Circle_Info         info) noexcept;
	void immediate(Rectangle_Info      info) noexcept;
	void immediate(Arrow_Info          info) noexcept;
	void immediate(Line_Info           info) noexcept;
}
