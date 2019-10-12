&emcc "build/imgui_demo.bc"`
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
	-s ASSERTIONS=2 -s ALLOW_MEMORY_GROWTH=1 -WASM=1 `
	--embed-file ./assets/ --emrun