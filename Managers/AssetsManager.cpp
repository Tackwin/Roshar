#include "AssetsManager.hpp"
#include "OS/file.hpp"

#include <assert.h>

using namespace asset;

namespace asset {
	Store_t Store;
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
		return false;
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
[[nodiscard]] Key Store_t::make_texture() noexcept {
	auto k = xstd::uuid();

	textures.emplace(k, Asset_t<sf::Texture>{});

	return k;
}

void Store_t::monitor_texture(Key k, Texture_Callback F) noexcept {
	file::monitor_file(textures.at(k).path, [&] { F(textures.at(k).asset); });
}
