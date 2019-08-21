#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <optional>
#include <memory>

namespace asset {
	using Key = std::uint64_t;

	template<typename T>
	struct Asset_t {
		T asset;
		std::filesystem::path path;
	};
	struct Known_Textures {
		inline static Key Key_Item{ 1 };
		inline static Key Rock{ 2 };
	};

	struct Store_t {

		std::unordered_map<std::string, std::uint64_t> textures_loaded;
		std::unordered_map<std::uint64_t, Asset_t<sf::Texture>> textures;
	
		[[nodiscard]] sf::Texture& get_texture(Key k) noexcept;
		[[nodiscard]] Key make_texture() noexcept;
		[[nodiscard]] std::optional<Key> load_texture(std::filesystem::path path) noexcept;
		[[nodiscard]] bool load_texture(Key k,  std::filesystem::path path) noexcept;
		void monitor_path(std::filesystem::path dir) noexcept;

		void load_known_textures() noexcept;
	};
	extern Store_t Store;

}

