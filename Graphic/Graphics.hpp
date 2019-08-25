#pragma once

#include "Managers/AssetsManager.hpp"
#include "Math/Vector.hpp"

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

	void sprite(Vector2f pos, Vector2f size, asset::Key texture) noexcept;
	void sprite(Sprite_Info info) noexcept;

}
