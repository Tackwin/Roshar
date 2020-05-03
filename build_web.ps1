Set-PSDebug -Trace 1

emcc "./src/External/web/imgui_demo.cpp"     -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_demo.bc
emcc "./src/External/web/imgui_draw.cpp"     -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_draw.bc
emcc "./src/External/web/imgui.cpp"          -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui.bc
emcc "./src/imgui_impl_web.cpp"              -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_impl_web.bc
emcc "./src/Entry/web_main.cpp"              -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/web_main.bc
emcc "./src/Entry/main.cpp"                  -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/main.bc
emcc "./src/Graphic/Animation.cpp"           -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Animation.bc
emcc "./src/Graphic/FrameBuffer.cpp"         -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/FrameBuffer.bc
emcc "./src/Graphic/Graphics.cpp"            -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Graphics.bc
emcc "./src/Graphic/Particle.cpp"            -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Particle.bc
emcc "./src/Graphic/Shader.cpp"              -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Shader.bc
emcc "./src/Graphic/Font.cpp"                -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Font.bc
emcc "./src/Graphic/Texture.cpp"             -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Texture.bc
emcc "./src/Graphic/UI/Kit.cpp"              -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Kit.bc
emcc "./src/Managers/AssetsManager.cpp"      -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/AssetsManager.bc
emcc "./src/Managers/InputsManager.cpp"      -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/InputsManager.bc
emcc "./src/OS/Emscripten/file.cpp"          -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/file.bc
emcc "./src/OS/Emscripten/RealTimeIO.cpp"    -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/RealTimeIO.bc
emcc "./src/Collision.cpp"                   -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Collision.bc
emcc "./src/dyn_struct.cpp"                  -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/dyn_struct.bc
emcc "./src/Editor.cpp"                      -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Editor.bc
emcc "./src/Game.cpp"                        -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Game.bc
emcc "./src/global.cpp"                      -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/global.bc
emcc "./src/Level.cpp"                       -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Level.bc
emcc "./src/Player.cpp"                      -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Player.bc
emcc "./src/Formats.cpp"                     -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Formats.bc
emcc "./src/Time.cpp"                        -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Time.bc
emcc "./src/Profil/Profile.cpp"              -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Profile.bc
emcc "./src/Screens/PlayScreen.cpp"          -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/PlayScreen.bc
emcc "./src/Screens/ProfileSelection.cpp"    -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/ProfileSelection.bc
emcc "./src/Screens/Settings.cpp"            -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Settings.bc
emcc "./src/Screens/Start.cpp"               -O0 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Start.bc

./link_web.ps1

emrun --port 2356 ./build/Roshar.html