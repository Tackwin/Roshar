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

add_compile_definitions(NOMINMAX)
add_compile_definitions(WIN32)
add_compile_definitions(PROFILER)
add_compile_definitions(CONSOLE)

add_executable(Roshar
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Entry/win32_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Entry/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/imgui_impl_opengl3.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/imgui_impl_win32.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/imgui_impl_win32.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/FrameBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/FrameBuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Animation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Animation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Graphics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Graphics.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Particle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Particle.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Texture.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Shader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/Font.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphic/UI/kit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Managers/AssetsManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Managers/AssetsManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Managers/InputsManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Managers/InputsManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Screens/ProfileSelection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Screens/PlayScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Screens/Settings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Screens/Start.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Profil/Profile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/RandomData.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/Rectangle.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/Segment.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/Circle.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/Matrix.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/Vector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Math/Ray.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Memory/ValuePtr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OS/Emscripten/RealTimeIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OS/Windows/RealTimeIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OS/Emscripten/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OS/Windows/Process.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OS/Windows/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OS/RealTimeIO.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OS/OpenGL.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OS/file.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Profiler/Tracer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dyn_struct.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dyn_struct.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Collision.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Collision.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/constexpr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Formats.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Editor.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/global.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/global.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/macros.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Player.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Player.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Level.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Level.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Game.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Time.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Time.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xstd.hpp
)


find_package(GLEW REQUIRED)
target_link_libraries(Roshar PRIVATE GLEW::GLEW)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(Roshar PRIVATE imgui::imgui)