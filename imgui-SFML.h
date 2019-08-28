// Add this to your imconfig.h

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#ifndef IMGUI_SFML_EXPORT_H
#define IMGUI_SFML_EXPORT_H

#if IMGUI_SFML_SHARED_LIB
#if _WIN32
#ifdef IMGUI_SFML_EXPORTS
#define IMGUI_SFML_API __declspec(dllexport)
#define IMGUI_API __declspec(dllexport)
#else
#define IMGUI_SFML_API __declspec(dllimport)
#define IMGUI_API __declspec(dllexport)
#endif
#elif __GNUC__ >= 4
#define IMGUI_SFML_API __attribute__ ((visibility("default")))
#define IMGUI_API __attribute__ ((visibility("default")))
#else
#define IMGUI_SFML_API
#define IMGUI_API
#endif
#else
#define IMGUI_SFML_API
#define IMGUI_API
#endif

#endif

#define IM_VEC2_CLASS_EXTRA                                             \
    template <typename T>                                               \
    ImVec2(const sf::Vector2<T>& v) {                                   \
        x = static_cast<float>(v.x);                                    \
        y = static_cast<float>(v.y);                                    \
    }                                                                   \
                                                                        \
    template <typename T>                                               \
    operator sf::Vector2<T>() const {                                   \
        return sf::Vector2<T>(x, y);                                    \
    }

#define IM_VEC4_CLASS_EXTRA                                             \
    ImVec4(const sf::Color & c)                                         \
        : ImVec4(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f) {  \
    }                                                                   \
    operator sf::Color() const {                                        \
        return sf::Color(                                               \
            static_cast<sf::Uint8>(x * 255.f),                          \
            static_cast<sf::Uint8>(y * 255.f),                          \
            static_cast<sf::Uint8>(z * 255.f),                          \
            static_cast<sf::Uint8>(w * 255.f));                         \
    }

#define ImTextureID unsigned int