&emcc "./src/External/web/imgui_demo.cpp"     -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_demo.bc
&emcc "./src/External/web/imgui_draw.cpp"     -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_draw.bc
&emcc "./src/External/web/imgui.cpp"          -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui.bc
&emcc "./src/imgui_impl_web.cpp"              -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/imgui_impl_web.bc
&emcc "./src/Entry/web_main.cpp"              -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/web_main.bc
&emcc "./src/Graphic/Animation.cpp"           -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Animation.bc
&emcc "./src/Graphic/FrameBuffer.cpp"         -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/FrameBuffer.bc
&emcc "./src/Graphic/Graphics.cpp"            -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Graphics.bc
&emcc "./src/Graphic/Particle.cpp"            -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Particle.bc
&emcc "./src/Graphic/Shader.cpp"              -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Shader.bc
&emcc "./src/Graphic/Texture.cpp"             -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Texture.bc
&emcc "./src/Managers/AssetsManager.cpp"      -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/AssetsManager.bc
&emcc "./src/Managers/InputsManager.cpp"      -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/InputsManager.bc
&emcc "./src/OS/Emscripten/file.cpp"          -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/file.bc
&emcc "./src/OS/Emscripten/RealTimeIO.cpp"    -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/RealTimeIO.bc
&emcc "./src/Collision.cpp"                   -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Collision.bc
&emcc "./src/dyn_struct.cpp"                  -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/dyn_struct.bc
&emcc "./src/Editor.cpp"                      -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Editor.bc
&emcc "./src/Game.cpp"                        -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Game.bc
&emcc "./src/global.cpp"                      -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/global.bc
&emcc "./src/Level.cpp"                       -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Level.bc
&emcc "./src/Player.cpp"                      -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Player.bc
&emcc "./src/Time.cpp"                        -O2 -Isrc/External/web/ -Isrc -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -std=c++17 -D WEB -D ES -o build/Time.bc

&./link_web.ps1

&emrun --port 2356 ./build/Roshar.html