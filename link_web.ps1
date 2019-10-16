&emcc -O1 "build/imgui_demo.bc"`
	"build/imgui_draw.bc"`
	"build/imgui.bc"`
	"build/imgui_impl_web.bc"`
	"build/web_main.bc"`
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
	"build/global.bc"`
	"build/Level.bc"`
	"build/Player.bc"`
	"build/Time.bc"`
	-o "build/Roshar.js"`
	-s ASSERTIONS=0 -s TOTAL_STACK=32MB -s TOTAL_MEMORY=128MB `
	-s USE_WEBGL2=1 -s DEMANGLE_SUPPORT=0 -s SAFE_HEAP=0 -s WASM=1 `
	-s FULL_ES3=1 -s FULL_ES2=1 -s GL_ASSERTIONS=0 -O1 `
	--embed-file ./assets/ --emrun