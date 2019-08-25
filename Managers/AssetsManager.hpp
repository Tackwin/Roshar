#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <optional>
#include <memory>

#include "Graphic/Shader.hpp"
#include "Graphic/Texture.hpp"

namespace asset {
	using Key = std::uint64_t;

	template<typename T>
	struct Asset_t {
		T asset;
		std::filesystem::path path;
	};

	template<>
	struct Asset_t<Shader> {
		Shader asset;
		std::filesystem::path vertex;
		std::filesystem::path fragment;
	};

	struct Known_Textures {
		inline static Key Key_Item{ 1 };
		inline static Key Rock{ 2 };
	};
	struct Known_Shaders {
		inline static Key Default{ 1 };
	};

	struct Store_t {

		std::unordered_map<std::string, std::uint64_t> textures_loaded;

		std::unordered_map<std::uint64_t, Asset_t<sf::Texture>> textures;
		std::unordered_map<std::uint64_t, Asset_t<Shader>> shaders;
		std::unordered_map<std::uint64_t, Asset_t<Texture>> my_textures;

		[[nodiscard]] sf::Texture& get_texture(Key k) noexcept;
		[[nodiscard]] Key make_texture() noexcept;
		[[nodiscard]] std::optional<Key> load_texture(std::filesystem::path path) noexcept;
		[[nodiscard]] bool load_texture(Key k, std::filesystem::path path) noexcept;

		[[nodiscard]] Texture& get_my_texture(Key k) noexcept;
		[[nodiscard]] Key make_my_texture() noexcept;
		[[nodiscard]] std::optional<Key> load_my_texture(std::filesystem::path path) noexcept;
		[[nodiscard]] bool load_my_texture(Key k, std::filesystem::path path) noexcept;

		[[nodiscard]] Shader& get_shader(Key k) noexcept;
		[[nodiscard]] std::optional<Key> load_shader(
			std::filesystem::path vertex, std::filesystem::path fragment
		) noexcept;
		[[nodiscard]] bool load_shader(
			Key k, std::filesystem::path vertex, std::filesystem::path fragment
		) noexcept;

		void monitor_path(std::filesystem::path dir) noexcept;

		void load_known_textures() noexcept;
		void load_known_shaders() noexcept;
	};
	extern Store_t Store;

}

