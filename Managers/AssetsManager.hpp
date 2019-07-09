#pragma once
#include <unordered_map>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "./../3rd/json.hpp"

class AssetsManager {
private:
	AssetsManager();
	~AssetsManager();

public:
	static bool haveTexture(const std::string& key);
	static bool loadTexture(const std::string& key, const std::string& path);
	static /*const*/ sf::Texture& getTexture(const std::string& key);  // TODO
															// is const really nedded here ?
															// i don't want somebody to change a texture for everyone else
															// but then i can't set the smoothness of a texture for instance
	static bool haveImage(const std::string& key);
	static bool loadImage(const std::string& key, const std::string& path);
	static const sf::Image& getImage(const std::string& key);

	static bool haveFont(const std::string& key);
	static bool loadFont(const std::string& key, const std::string& path);
	static const sf::Font& getFont(const std::string& key);

	static bool haveSound(const std::string& key);
	static bool loadSound(const std::string& key, const std::string& path);
	static const sf::SoundBuffer& getSound(const std::string& key);

	static void invalidateJson(const std::string& key) noexcept;
	static bool haveJson(const std::string& key);
	static bool loadJson(const std::string& key, const std::string& path);
	static const nlohmann::json& getJson(const std::string& key);

	static bool haveMusic(const std::string& key);
	static bool openMusic(const std::string& key, const std::string& path);
	static sf::Music& getMusic(const std::string& key);

	static size_t getSize();

private:
	static std::unordered_map<std::string, sf::SoundBuffer> _sounds;
	static std::unordered_map<std::string, nlohmann::json> _jsons;
	static std::unordered_map<std::string, sf::Texture> _textures;
	static std::unordered_map<std::string, sf::Music> _musics;
	static std::unordered_map<std::string, sf::Image> _images;
	static std::unordered_map<std::string, sf::Font> _fonts;
};

using AM = AssetsManager;
