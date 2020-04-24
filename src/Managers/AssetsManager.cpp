#include "AssetsManager.hpp"
#include "OS/file.hpp"

#include "../Level.hpp"

#include "OS/OpenGL.hpp"
#include "Assets.hpp"

#include "Profil/Profile.hpp"

using namespace asset;

namespace asset {
	Store_t Store;
}

[[nodiscard]] Texture* Store_t::get_normal(Key k) const noexcept {
	return textures.at(k).normal ? textures.at(k).normal.get() : nullptr;
}

[[nodiscard]] Texture& Store_t::get_albedo(Key k) noexcept {
	return textures.at(k).albedo;
}
[[nodiscard]] Shader& Store_t::get_shader(Key k) noexcept {
	return shaders.at(k).asset;
}

[[nodiscard]] std::optional<Key> Store_t::load_texture(std::filesystem::path path) noexcept {
	auto x = xstd::uuid();
	return load_texture(x, path) ? std::optional{ x } : std::nullopt;
}

[[nodiscard]] bool Store_t::load_texture(Key k, std::filesystem::path path) noexcept {
	auto& new_texture = textures[k];
	new_texture.path = path;
	bool loaded = new_texture.albedo.load_file(path);

	auto normal_path = path;
	normal_path.replace_filename(path.stem().string() + "_n.png");

	if (std::filesystem::is_regular_file(normal_path)) {
		new_texture.normal = std::make_unique<Texture>();
		new_texture.normal->load_file(normal_path);
	}

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
[[nodiscard]] std::optional<Key> Store_t::load_shader(
	std::filesystem::path vertex,
	std::filesystem::path fragment,
	std::filesystem::path geometry
) noexcept {
	auto x = xstd::uuid();
	return load_shader(x, vertex, fragment, geometry) ? std::optional{x} : std::nullopt;
}


[[nodiscard]] bool Store_t::load_shader(
	Key k, std::filesystem::path vertex, std::filesystem::path fragment
) noexcept {
	auto new_shader = Shader::create_shader(vertex, fragment);
	if (!new_shader) return false;

	Asset_t<Shader> s{
		std::move(*new_shader),
		std::filesystem::canonical(vertex),
		std::filesystem::canonical(fragment)
	};


	shaders.insert({ k, std::move(s) });

	return true;
}

[[nodiscard]] bool Store_t::load_shader(
	Key k,
	std::filesystem::path vertex,
	std::filesystem::path fragment,
	std::filesystem::path geometry
) noexcept {
	auto new_shader = Shader::create_shader(vertex, fragment, geometry);
	if (!new_shader) return false;

	Asset_t<Shader> s{
		std::move(*new_shader),
		std::filesystem::canonical(vertex),
		std::filesystem::canonical(fragment),
		std::filesystem::canonical(geometry)
	};

	shaders.insert({ k, std::move(s) });

	return true;
}

[[nodiscard]] Key Store_t::make_texture() noexcept {
	auto k = xstd::uuid();

	textures.emplace(k, Asset_t<Texture>{});

	return k;
}

[[nodiscard]] Animation_Sheet& Store_t::get_animation(Key k) noexcept {
	return animations.at(k).asset;
}
[[nodiscard]] std::optional<Key> Store_t::load_animation(std::filesystem::path path) noexcept {
	auto k = xstd::uuid();
	if (!load_animation(k, path)) return std::nullopt;
	return k;
}
[[nodiscard]] bool Store_t::load_animation(Key k, std::filesystem::path path) noexcept {
	auto opt_str = load_from_json_file(path);
	if (!opt_str) return false;

	animations[k].asset = (Animation_Sheet)* opt_str;
	animations[k].path = path;

	return true;
}

[[nodiscard]] Particle_System& Store_t::get_particle(Key k) noexcept {
	return particle_systems.at(k).asset;
}
[[nodiscard]] std::optional<Key> Store_t::load_particle(std::filesystem::path path) noexcept {
	auto k = xstd::uuid();
	if (!load_particle(k, path)) return std::nullopt;
	return k;
}
[[nodiscard]] bool Store_t::load_particle(Key k, std::filesystem::path path) noexcept {
	auto opt_str = load_from_json_file(path);
	if (!opt_str) return false;

	particle_systems[k].asset = (Particle_System)* opt_str;
	particle_systems[k].path = path;

	return true;
}


void Store_t::monitor_path(std::filesystem::path dir) noexcept {
#ifndef WEB
	static int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0
	};

	auto gl = wglCreateContextAttribsARB(
		(HDC)platform::handle_dc_window, (HGLRC)platform::main_opengl_context, attribs
	);

	file::monitor_dir(
		[gl] {
			if (!wglMakeCurrent(
				(HDC)platform::handle_dc_window, gl
			)) std::abort();
		},
		dir,
		[&, d = dir] (std::filesystem::path path) -> bool {
			if (stop) return true;
			std::lock_guard guard{ Main_Mutex };
			
			path = std::filesystem::canonical(Exe_Path / d / path);

			{
				// >SLOW(Tackwin)
				auto suffixes = xstd::split(path.stem().string(), "_");

				auto key = path.parent_path() / (suffixes.front() + path.extension().string());

				auto it = textures_loaded.find(key.string());
				
				if (it != END(textures_loaded) && textures.count(it->second)) {
					auto& asset = textures.at(it->second);

					if (suffixes.size() == 1) {
						asset.albedo.load_file(path.string());
						return false;
					}

					auto last = suffixes.back();
					if (last == "n") {
						if (!asset.normal) asset.normal = std::make_unique<Texture>();
						asset.normal->load_file(path.string());
					}
				}

			}

			for (auto& [_, x] : shaders){
				if (x.fragment == path) {
					if (!x.asset.load_fragment(path)) continue;

					if (x.asset.build_shaders()) printf("Rebuilt shader.\n");
					else printf("Faield to rebuild shader.\n");
				}
				else if (x.vertex == path) {

					if (!x.asset.load_vertex(path)) continue;

					if (x.asset.build_shaders()) printf("Rebuilt shader.\n");
					else printf("Faield to rebuild shader.\n");
				}
				else if (x.geometry == path) {

					if (!x.asset.load_geometry(path)) continue;

					if (x.asset.build_shaders()) printf("Rebuilt shader.\n");
					else printf("Failed to rebuild shader.\n");
				}
			}
			return false;
		}
	);
#endif
}

void Store_t::load_known_textures() noexcept {

	std::optional<Key> opt;

#define X(str, x)\
	printf("Loading " str " ... ");\
	opt = load_texture(str);\
	if (opt) {\
		Texture_Id::x = *opt;\
		printf("sucess :) !\n");\
	}\
	else {\
		printf("failed :( !\n");\
	}

	X("assets/textures/basic_binding_indicator_head.png", Basic_Binding_Indicator_Head);
	X("assets/textures/basic_binding_indicator_body.png", Basic_Binding_Indicator_Body);
	X("assets/textures/dust_sheet.png",                   Dust_Sheet                  );
	X("assets/textures/indicator.png",                    Indicator                   );
	X("assets/textures/poulp-sheet.png",                  Guy_Sheet                   );
	X("assets/textures/card_plus.png",                    Card_Plus                   );
	X("assets/textures/rock.png",                         Rock                        );
	X("assets/textures/key.png",                          Key_Item                    );

#undef X
}

void Store_t::load_known_fonts() noexcept {
	std::optional<Key> opt;

#define X(str, x)\
	printf("Loading " str " ... ");\
	opt = load_font(str);\
	if (opt) {\
		Font_Id::x = *opt;\
		printf("sucess :) !\n");\
	}\
	else {\
		printf("failed :( !\n");\
	}

	X("assets/fonts/consolas_regular_100.json", Consolas);

#undef X
}

void Store_t::load_known_shaders() noexcept {

	std::optional<Key> opt;


#define X(f, v, g, x)\
	printf("Loading " #x " shader... ");\
	if (*g == '\0') opt = load_shader(f, v);\
	else opt = load_shader(f, v, g);\
	if (opt) {\
		Shader_Id::x = *opt;\
		printf("sucess :) !\n");\
	}\
	else {\
		printf("failed :( !\n");\
	}

	X("assets/shaders/default.vertex", "assets/shaders/default.fragment", "", Default);
	X("assets/shaders/light.vertex",   "assets/shaders/light.fragment",   "", Light  );
	X("assets/shaders/hdr.vertex",     "assets/shaders/hdr.fragment",     "", HDR    );
	X(
		"assets/shaders/line.vertex",
		"assets/shaders/hdr.fragment",
		"assets/shaders/line.geometry",
		Line
	);
#undef X
}

void Store_t::load_from_config(std::filesystem::path config_path) noexcept {
	Animation_Id::Guy = xstd::uuid();
	auto f = [&, config_path] () -> bool {
		if (stop) return true;
		auto opt = load_from_json_file(Exe_Path / config_path);
		if (!opt) {
			printf("Erreur.\n");
			return false;
		}

		dyn_struct config = std::move(*opt);

		std::lock_guard guard{ Main_Mutex };
		if (has(config, "animations")) {
			const auto& anim = config["animations"];

			if (has(anim, "Guy")) {
				printf("Load Guy animations.\n");
				animations[Animation_Id::Guy].asset = (Animation_Sheet)anim["Guy"];
			}
		}
		if (has(config, "textures")){
			for (const auto& [key, value] : iterate_structure(config["textures"])) {
				auto id = load_texture((std::string)value);
				if (id) texture_string_map.insert({(std::string)key, *id});
			}
		}
		if (has(config, "particles")) {
			const auto& part = config["particles"];

			if (has(part, "Player_Foot")) {
				printf("Load Player_Foot particle system.\n");
				particle_systems[Particle_Id::Player_Foot].asset =
					(Particle_System)part["Player_Foot"];
			}
		}

		return false;
	};
#ifndef WEB
	file::monitor_file(std::filesystem::canonical(Exe_Path / config_path), f);
#endif
	f();
}


[[nodiscard]] Font& Store_t::get_font(Key k) noexcept {
	return fonts.at(k).asset;
}

[[nodiscard]] bool Store_t::load_font(Key k, std::filesystem::path path) noexcept {
	Asset_t<Font> font;
	font.path = path;

	auto opt = load_from_json_file(path);
	if (!opt) return false;
	font.asset.info = (Font::Font_Info)*opt;

	auto it = std::find_if(
		BEG_END(textures),
		[p = font.asset.info.texture_path](const std::pair<const asset::Key, Asset_t<Texture>>& x) {
			return x.second.path == p;
		}
	);

	if (it == END(textures)) {
		auto opt_key = load_texture(path.parent_path() / font.asset.info.texture_path);
		if (!opt_key) return false;
		font.asset.texture_id = *opt_key;
	} else {
		font.asset.texture_id = it->first;
	}
	
	get_albedo(font.asset.texture_id).set_resize_filter(Texture::Filter::Linear);
	
	fonts.emplace(k, std::move(font));
	
	return true;
}

[[nodiscard]] std::optional<Key> Store_t::load_font(std::filesystem::path path) noexcept{
	asset::Key k = xstd::uuid();
	if (auto opt = load_font(k, path)) return k;
	return std::nullopt;
}

void Store_t::save_profiles(std::filesystem::path file, std::vector<Profile>& profiles) noexcept {
	auto opt = load_from_json_file(file);
	if (!opt) return;

	auto& str = *opt;
	str["profiles"] = profiles;
	save_to_json_file(str, file);
}

