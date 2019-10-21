#pragma once

namespace asset {
	using Key = std::uint64_t;

	struct Texture_Id {
		inline static Key Key_Item{ 1 };
		inline static Key Rock{ 2 };
		inline static Key Basic_Binding_Indicator_Body{ 3 };
		inline static Key Basic_Binding_Indicator_Head{ 4 };
		inline static Key Indicator{ 5 };
		inline static Key Guy_Sheet{ 6 };
		inline static Key Dust_Sheet{ 7 };
	};
	struct Shader_Id {
		inline static Key Default{ 1 };
		inline static Key Light{ 2 };
		inline static Key HDR{ 3 };
	};
	struct Animation_Id {
		inline static Key Guy{ 1 };
	};
	struct Particle_Id {
		inline static Key Player_Foot{ 1 };
	};
}
