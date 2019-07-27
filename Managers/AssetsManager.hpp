#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <optional>

namespace asset {
	using Key = std::uint64_t;
	using Texture_Callback = std::function<void(sf::Texture&)>;

	template<typename T>
	struct Asset_t {
		T asset;
		std::filesystem::path path;
	};

	struct Store_t {
		std::unordered_map<std::string, std::uint64_t> textures_loaded;
		std::unordered_map<std::uint64_t, Asset_t<sf::Texture>> textures;
	
		[[nodiscard]] std::optional<Key> load_texture(std::filesystem::path path) noexcept;
		void monitor_texture(Key k, Texture_Callback F) noexcept;
	};
	extern Store_t Store;

}

