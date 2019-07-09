// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "AssetsManager.hpp"
#include "Common.hpp"

#include <filesystem>
#include <cassert>
#include <fstream>

std::unordered_map<std::string, sf::SoundBuffer> AM::_sounds;
std::unordered_map<std::string, nlohmann::json> AM::_jsons;
std::unordered_map<std::string, sf::Texture> AM::_textures;
std::unordered_map<std::string, sf::Music> AM::_musics;
std::unordered_map<std::string, sf::Image> AM::_images;
std::unordered_map<std::string, sf::Font> AM::_fonts;

#ifdef _WIN32
#include <Windows.h>
void stubSetConsoleTextAttribute(HANDLE h, WORD w) {
	SetConsoleTextAttribute(h, w);
}
#else
void stubSetConsoleTextAttibute() {
	static bool flag = false;
	if (!flag) {
		printf("Color console not yet supported!\n");
		flag = true;
	}
};
#endif

AssetsManager::AssetsManager() {
}
AssetsManager::~AssetsManager() {
}

bool AssetsManager::haveTexture(const std::string& key) {
	return _textures.find(key) != std::end(_textures);
}
bool AssetsManager::loadTexture(const std::string& key, const std::string &path) {
	if (_textures.find(key) != std::end(_textures))
		return true;

	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_BLUE
	);
	std::printf("Loading: ");
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	auto &ref = _textures[key];
	ref.setSmooth(true);
	std::printf("%s: %s ", key.c_str(), path.c_str());

	const bool loaded = ref.loadFromFile(path);
	if(!loaded) {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_RED
		);
		printf(" Couldn't load file /!\\\n");
	}
	else {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_GREEN
		);
		printf(" Succes !\n");
	}
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	return loaded;
}

/*const*/ sf::Texture& AssetsManager::getTexture(const std::string &key) {
	auto it = _textures.find(key);
	assert(it != std::end(_textures) && "Texture don't exist");
	return it->second;
}

bool AssetsManager::haveImage(const std::string& key) {
	return _images.find(key) != std::end(_images);
}
bool AssetsManager::loadImage(const std::string &key, const std::string &path) {
	if (_images.find(key) != std::end(_images))
		return true;

	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_BLUE
	);
	std::printf("Loading: ");
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	std::printf("%s: %s ", key.c_str(), path.c_str());
	auto& ref = _images[key];

	const bool loaded = ref.loadFromFile(path);
	if(!loaded) {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_RED
		);
		printf(" Couldn't load file /!\\\n");
	} else {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_GREEN
		);
		printf(" Succes !\n");
	}
	SetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	return loaded;
}

const sf::Image& AssetsManager::getImage(const std::string &key) {
	auto it = _images.find(key);
	assert(it != std::end(_images) && "Image don't exist");
	return it->second;
}

bool AssetsManager::haveFont(const std::string& key) {
	return _fonts.find(key) != std::end(_fonts);
}
bool AssetsManager::loadFont(const std::string &key, const std::string &path) {
	if (_fonts.find(key) != std::end(_fonts))
		return true;
	
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_BLUE
	);
	std::printf("Loading: ");
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	std::printf("%s: %s ", key.c_str(), path.c_str());
	auto& ref = _fonts[key];

	const bool loaded = ref.loadFromFile(path);
	if(!loaded) {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_RED
		);
		printf(" Couldn't load file /!\\\n");
	} else {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_GREEN
		);
		printf(" Succes !\n");
	}
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	return loaded;
}

const sf::Font& AssetsManager::getFont(const std::string &key) {
	auto it = _fonts.find(key);
	assert(it != std::end(_fonts) && "Font don't exist");
	return it->second;
}

bool AssetsManager::haveSound(const std::string& key) {
	return _sounds.find(key) != std::end(_sounds);
}
bool AssetsManager::loadSound(const std::string &key, const std::string &path) {
	if (_sounds.find(key) != std::end(_sounds))
		return true;
	
	stubSetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
	std::printf("Loading: ");
	stubSetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::printf("%s: %s ", key.c_str(), path.c_str());
	auto& ref = _sounds[key];

	const bool loaded = ref.loadFromFile(path);
	if (!loaded) {
		stubSetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		printf(" Couldn't load file /!\\\n");
	}
	else {
		stubSetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		printf(" Succes !\n");
	}
	stubSetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return loaded;
}

const sf::SoundBuffer& AssetsManager::getSound(const std::string &key) {
	auto it = _sounds.find(key);
	assert(it != std::end(_sounds) && "Sound don't exist");
	return it->second;
}

void AssetsManager::invalidateJson(const std::string& key) noexcept {
	assert(AM::haveJson(key));

	_jsons.erase(key);
}
bool AssetsManager::haveJson(const std::string& key) {
	return _jsons.find(key) != std::end(_jsons);
}
bool AssetsManager::loadJson(const std::string &key, const std::string &path) {
	if (_jsons.find(key) != std::end(_jsons))
		return true;

	stubSetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
	std::printf("Loading: ");
	stubSetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::printf("%s: %s ", key.c_str(), path.c_str());

	std::ifstream stream;
	std::filesystem::current_path(EXE_DIR);
	stream.open(path);
	const bool loaded = stream.is_open() && stream.good();

	if (!loaded) {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_RED
		);
		printf(" Couldn't load file /!\\\n");
	}
	else {
		_TRY_BEGIN
			stream >> _jsons[key];
			stubSetConsoleTextAttribute(
				GetStdHandle(STD_OUTPUT_HANDLE), 
				FOREGROUND_GREEN
			);
			printf(" Succes !\n");
#if _HAS_EXCEPTIONS
		_CATCH(const std::exception& e)
#else
		_CATCH_ALL
			struct { const char* what() { return nullptr; } } e;
#endif
			stubSetConsoleTextAttribute(
				GetStdHandle(STD_OUTPUT_HANDLE), 
				FOREGROUND_RED
			);
			printf(" Couldn't load file /!\\ err: %s \n", e.what());
		_CATCH_END
	}
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	return loaded;
}

const nlohmann::json& AssetsManager::getJson(const std::string &key) {
	auto it = _jsons.find(key);
	assert(it != std::end(_jsons) && "Json don't exist");
	return it->second;
}

bool AssetsManager::haveMusic(const std::string& key) {
	return _musics.find(key) != std::end(_musics);
}
bool AssetsManager::openMusic(const std::string &key, const std::string &path) {
	if (_musics.find(key) != std::end(_musics))
		return true;
	
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_BLUE
	);
	std::printf("Loading: ");
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	std::printf("%s: %s ", key.c_str(), path.c_str());
	auto& ref = _musics[key];

	const bool loaded = ref.openFromFile(path);
	if(!loaded) {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_RED
		);
		printf(" Couldn't load file /!\\\n");
	} else {
		stubSetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE), 
			FOREGROUND_GREEN
		);
		printf(" Succes !\n");
	}
	stubSetConsoleTextAttribute(
		GetStdHandle(STD_OUTPUT_HANDLE), 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
	);
	return loaded;
}

sf::Music& AssetsManager::getMusic(const std::string &key) {
	auto it = _musics.find(key);
	assert(it != std::end(_musics) && "Music don't exist");
	return it->second;
}

// that's useless there is no information here, 
// implement this when you have your own library
size_t AM::getSize() {
	size_t s = 0u;

	s += _sounds.size() * sizeof(decltype(_sounds)::mapped_type);
	s += _jsons.size() * sizeof(decltype(_jsons)::mapped_type);
	s += _textures.size() * sizeof(decltype(_textures)::mapped_type);
	s += _musics.size() * sizeof(decltype(_musics)::mapped_type);
	s += _images.size() * sizeof(decltype(_images)::mapped_type);
	s += _fonts.size() * sizeof(decltype(_fonts)::mapped_type);

	return s;
}
