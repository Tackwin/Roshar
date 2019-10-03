#include "AssetsManager.hpp"
#include "OS/file.hpp"

#include <GL/glew.h>
#include <GL/wglew.h>
#include <Windows.h>
#include <cstdlib>

#include "../Level.hpp"

#include "Graphic/OpenGL.hpp"
#include "Assets.hpp"

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

static int attribs[] = {
	WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
	WGL_CONTEXT_MINOR_VERSION_ARB, 0,
	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
	0
};

void Store_t::monitor_path(std::filesystem::path dir) noexcept {
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
		[&, d = dir] (std::filesystem::path path) {
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
						return;
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
		Texture_Id::x = *opt;\
		printf("sucess :) !\n");\
	}\
	else {\
		printf("failed :( !\n");\
	}

	X("textures/basic_binding_indicator_head.png", Basic_Binding_Indicator_Head);
	X("textures/basic_binding_indicator_body.png", Basic_Binding_Indicator_Body);
	X("textures/indicator.png",                    Indicator                   );
	X("textures/guy_sheet.png",                    Guy_Sheet                   );
	X("textures/rock.png",                         Rock                        );
	X("textures/key.png",                          Key_Item                    );

#undef X
}

void Store_t::load_known_shaders() noexcept {

	std::optional<Key> opt;


#define X(f, v, x)\
	printf("Loading " #x " shader... ");\
	opt = load_shader(f, v);\
	if (opt) {\
		Shader_Id::x = *opt;\
		printf("sucess :) !\n");\
	}\
	else {\
		printf("failed :( !\n");\
	}

	X("shaders/default.vertex", "shaders/default.fragment", Default);
	X("shaders/light.vertex",   "shaders/light.fragment",   Light  );
	X("shaders/hdr.vertex",     "shaders/hdr.fragment",     HDR    );
#undef X
}

void Store_t::load_from_config(std::filesystem::path config_path) noexcept {
	Animation_Id::Guy = xstd::uuid();
	auto f = [&, config_path] {
		auto opt = load_from_json_file(config_path);
		if (!opt) {
			printf("Erreur.\n");
			return;
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
		if (has(config, "particles")) {
			const auto& part = config["particles"];

			if (has(part, "Sample")) {
				printf("Load Sample particle system.\n");
				particle_systems[Particle_Id::Sample].asset = (Particle_System)part["Sample"];
			}
		}
	};
	file::monitor_file(std::filesystem::canonical(Exe_Path / config_path), f);
	f();
}


