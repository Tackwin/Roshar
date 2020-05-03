emcc -O1 "build/imgui_demo.bc"`
	"build/imgui_draw.bc"`
	"build/imgui.bc"`
	"build/imgui_impl_web.bc"`
	"build/web_main.bc"`
	"build/main.bc"`
	"build/Animation.bc"`
	"build/FrameBuffer.bc"`
	"build/Graphics.bc"`
	"build/Particle.bc"`
	"build/Shader.bc"`
	"build/Texture.bc"`
	"build/AssetsManager.bc"`
	"build/InputsManager.bc"`
	"build/file.bc"`
	"build/RealTimeIO.bc"`
	"build/Collision.bc"`
	"build/dyn_struct.bc"`
	"build/Editor.bc"`
	"build/Game.bc"`
	"build/Font.bc"`
	"build/Texture.bc"`
	"build/Kit.bc"`
	"build/global.bc"`
	"build/Level.bc"`
	"build/Player.bc"`
	"build/Time.bc"`
	"build/Formats.bc"`
	"build/Profile.bc"`
	"build/PlayScreen.bc"`
	"build/ProfileSelection.bc"`
	"build/Settings.bc"`
	"build/Start.bc"`
	-o "build/Roshar.js"`
	-s ASSERTIONS=2 -s TOTAL_STACK=32MB -s TOTAL_MEMORY=128MB `
	-s USE_WEBGL2=1 -s DEMANGLE_SUPPORT=0 -s SAFE_HEAP=1 -s WASM=1 `
	-s FULL_ES3=1 -s FULL_ES2=1 -s GL_ASSERTIONS=2 -O0 `
	--embed-file ./assets/ --emrun