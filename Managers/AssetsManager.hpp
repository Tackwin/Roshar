#pragma once

#include <unordered_map>
#include <filesystem>
#include <functional>
#include <optional>
#include <memory>

#include "Graphic/Shader.hpp"
#include "Graphic/Texture.hpp"
#include "Graphic/Animation.hpp"

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

	template<>
	struct Asset_t<Texture> {
		Texture albedo;
		std::filesystem::path path;
		std::unique_ptr<Texture> normal;
	};

	struct Store_t {

		std::unordered_map<std::string, std::uint64_t> textures_loaded;

		std::unordered_map<std::uint64_t, Asset_t<Shader>> shaders;
		std::unordered_map<std::uint64_t, Asset_t<Texture>> textures;
		std::unordered_map<std::uint64_t, Asset_t<Animation>> animations;

		[[nodiscard]] Texture* get_normal(Key k) const noexcept;
		[[nodiscard]] Texture& get_albedo(Key k) noexcept;
		[[nodiscard]] Key make_texture() noexcept;
		[[nodiscard]] std::optional<Key> load_texture(std::filesystem::path path) noexcept;
		[[nodiscard]] bool load_texture(Key k, std::filesystem::path path) noexcept;
		
		[[nodiscard]] Shader& get_shader(Key k) noexcept;
		[[nodiscard]] std::optional<Key> load_shader(
			std::filesystem::path vertex, std::filesystem::path fragment
		) noexcept;
		[[nodiscard]] bool load_shader(
			Key k, std::filesystem::path vertex, std::filesystem::path fragment
		) noexcept;

		[[nodiscard]] Animation& get_animation(Key k) noexcept;
		[[nodiscard]] std::optional<Key> load_animation(std::filesystem::path path) noexcept;
		[[nodiscard]] bool load_animation(Key k, std::filesystem::path path) noexcept;

		void monitor_path(std::filesystem::path dir) noexcept;

		void load_known_textures() noexcept;
		void load_known_shaders() noexcept;
		void load_from_config(std::filesystem::path config_path) noexcept;
	};
	extern Store_t Store;

}

