&emcc "./src/External/web/imgui_demo.cpp"     -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_demo.bc
&emcc "./src/External/web/imgui_draw.cpp"     -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_draw.bc
&emcc "./src/External/web/imgui.cpp"          -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui.bc
&emcc "./src/imgui_impl_web.cpp"              -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_impl_web.bc
&emcc "./src/Entry/web_main.cpp"              -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/web_main.bc
&emcc "./src/Graphic/Animation.cpp"           -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Animation.bc
&emcc "./src/Graphic/FrameBuffer.cpp"         -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/FrameBuffer.bc
&emcc "./src/Graphic/Graphics.cpp"            -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Graphics.bc
&emcc "./src/Graphic/Particle.cpp"            -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Particle.bc
&emcc "./src/Graphic/Shader.cpp"              -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Shader.bc
&emcc "./src/Graphic/Texture.cpp"             -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Texture.bc
&emcc "./src/Managers/AssetsManager.cpp"      -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/AssetsManager.bc
&emcc "./src/Managers/InputsManager.cpp"      -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/InputsManager.bc
&emcc "./src/OS/Emscripten/file.cpp"          -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/file.bc
&emcc "./src/OS/Emscripten/RealTimeIO.cpp"    -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/RealTimeIO.bc
&emcc "./src/Collision.cpp"                   -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Collision.bc
&emcc "./src/dyn_struct.cpp"                  -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/dyn_struct.bc
&emcc "./src/Editor.cpp"                      -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Editor.bc
&emcc "./src/Game.cpp"                        -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Game.bc
&emcc "./src/global.cpp"                      -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/global.bc
&emcc "./src/Level.cpp"                       -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Level.bc
&emcc "./src/Player.cpp"                      -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Player.bc
&emcc "./src/Time.cpp"                        -g -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Time.bc

&./link_web.ps1

&emrun --port 2356 ./build/Roshar.html