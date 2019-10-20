cmake_minimum_required(VERSION 3.15)
project(Roshar)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

if(MSVC)
    add_definitions(/FI"global.hpp")
    add_definitions(/FI"xstd.hpp")
    add_definitions(/FI"constexpr.hpp")
    add_definitions(/FI"macros.hpp")
else()
    # GCC or Clang
    add_definitions(-include global.hpp)
    add_definitions(-include xstd.hpp)
    add_definitions(-include constexpr.hpp)
    add_definitions(-include macros.hpp)
endif()

add_executable(Roshar
        src/Entry/win32_main.cpp
        src/Graphic/Animation.cpp
        src/Graphic/Animation.hpp
        src/Graphic/FrameBuffer.cpp
        src/Graphic/FrameBuffer.hpp
        src/Graphic/Graphics.cpp
        src/Graphic/Graphics.hpp
        src/Graphic/imgui_impl_opengl3.cpp
        src/Graphic/imgui_impl_opengl3.hpp
        src/Graphic/imgui_impl_win32.cpp
        src/Graphic/imgui_impl_win32.hpp
        src/Graphic/Particle.cpp
        src/Graphic/Particle.hpp
        src/Graphic/Shader.cpp
        src/Graphic/Shader.hpp
        src/Graphic/Texture.cpp
        src/Graphic/Texture.hpp
        src/Managers/AssetsManager.cpp
        src/Managers/AssetsManager.hpp
        src/Managers/InputsManager.cpp
        src/Managers/InputsManager.hpp
        src/Math/Circle.hpp
        src/Math/Matrix.hpp
        src/Math/RandomData.hpp
        src/Math/Ray.hpp
        src/Math/Rectangle.hpp
        src/Math/Segment.hpp
        src/Math/Vector.hpp
        src/Memory/ValuePtr.hpp
        src/OS/Emscripten/file.cpp
        src/OS/Emscripten/RealTimeIO.cpp
        src/OS/Windows/file.cpp
        src/OS/Windows/RealTimeIO.cpp
        src/OS/file.hpp
        src/OS/OpenGL.hpp
        src/OS/RealTimeIO.hpp
        src/Assets.hpp
        src/Collision.cpp
        src/Collision.hpp
        src/constexpr.hpp
        src/dyn_struct.cpp
        src/dyn_struct.hpp
        src/Editor.cpp
        src/Editor.hpp
        src/Game.cpp
        src/Game.hpp
        src/global.cpp
        src/global.hpp
        src/imconfig.h
        src/Level.cpp
        src/Level.hpp
        src/macros.hpp
        src/Player.cpp
        src/Player.hpp
        src/Time.cpp
        src/Time.hpp
        src/xstd.hpp)
