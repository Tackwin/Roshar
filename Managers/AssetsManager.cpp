#include "AssetsManager.hpp"
#include "OS/file.hpp"

#include <assert.h>

#include "../Level.hpp"

using namespace asset;

namespace asset {
	Store_t Store;
}

[[nodiscard]] sf::Texture& Store_t::get_texture(Key k) noexcept {
	return textures.at(k).asset;
}
[[nodiscard]] Texture& Store_t::get_my_texture(Key k) noexcept {
	return my_textures.at(k).asset;
}
[[nodiscard]] Shader& Store_t::get_shader(Key k) noexcept {
	return shaders.at(k).asset;
}

[[nodiscard]] std::optional<Key> Store_t::load_texture(std::filesystem::path path) noexcept {
	auto k = xstd::uuid();

	auto new_texture = Asset_t<sf::Texture>{};
	new_texture.path = path;
	bool loaded = new_texture.asset.loadFromFile(path.string());

	if (loaded) {
		textures[k] = std::move(new_texture);
		textures_loaded[std::filesystem::canonical(path).string()] = k;
		return k;
	}
	else {
		return std::nullopt;
	}
}

[[nodiscard]] bool Store_t::load_texture(Key k, std::filesystem::path path) noexcept {
	auto& new_texture = textures.at(k);
	new_texture.path = path;
	bool loaded = new_texture.asset.loadFromFile(path.string());

	if (loaded) {
		textures_loaded[std::filesystem::canonical(path).string()] = k;
		return true;
	}
	else {
		return false;
	}
}

[[nodiscard]] std::optional<Key> Store_t::load_my_texture(std::filesystem::path path) noexcept {
	auto x = xstd::uuid();
	return load_my_texture(x, path) ? std::optional{ x } : std::nullopt;
}

[[nodiscard]] bool Store_t::load_my_texture(Key k, std::filesystem::path path) noexcept {
	auto& new_texture = my_textures.at(k);
	new_texture.path = path;
	bool loaded = new_texture.asset.load_file(path.string());

	if (loaded) {
		textures_loaded[std::filesystem::canonical(path).string()] = k;
		return true;
	}
	else {
		return false;
	}
}

[[nodiscard]] std::optional<Key> Store_t::load_shader(
	std::filesystem::path vertex, std::filesystem::path fragment
) noexcept {
	auto x = xstd::uuid();
	return load_shader(x, vertex, fragment) ? std::optional{x} : std::nullopt;
}

[[nodiscard]] bool Store_t::load_shader(
	Key k, std::filesystem::path vertex, std::filesystem::path fragment
) noexcept {
	auto new_shader = Shader::create_shader(vertex, fragment);
	if (!new_shader) return false;

	Asset_t<Shader> s{
		std::move(*new_shader),
		vertex,
		fragment
	};


	shaders.insert({ k, std::move(s) });

	return true;
}

[[nodiscard]] Key Store_t::make_texture() noexcept {
	auto k = xstd::uuid();

	textures.emplace(k, Asset_t<sf::Texture>{});

	return k;
}

[[nodiscard]] Key Store_t::make_my_texture() noexcept {
	auto k = xstd::uuid();

	my_textures.emplace(k, Asset_t<Texture>{});

	return k;
}

void Store_t::monitor_path(std::filesystem::path dir) noexcept {
	file::monitor_dir(
		dir,
		[&, d = dir] (auto path) {
			std::lock_guard guard{ Main_Mutex };
			path = std::filesystem::canonical(Exe_Path / d / path);

			{
				auto it = textures_loaded.find(path.string());
				if (it != END(textures_loaded) && textures.count(it->second)) {
					textures.at(it->second).asset.loadFromFile(path.string());
				}
				if (it != END(textures_loaded) && my_textures.count(it->second)) {
					my_textures.at(it->second).asset.load_file(path.string());
				}
			}

		}
	);
}

void Store_t::load_known_textures() noexcept {
    
	std::optional<Key> opt;
    
#define X(str, x)\
	printf("Loading " str " ... ");\
	opt = load_texture(str);\
	if (opt) {\
		Known_Textures::x = *opt;\
		printf("sucess :) !\n");\
	}\
	else {\
		printf("failed :( !\n");\
	}

	X("textures/key.png", Key_Item);
	X("textures/rock.png", Rock);

#undef X
}
