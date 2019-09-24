$files = New-Object System.Collections.Generic.List[System.Object]

Get-ChildItem -Recurse -Path . -File | ForEach-Object {
	if ($_.Extension -eq ".cpp") {
		$files.Add($_.FullName)
	}
	if ($_.Extension -eq ".c") {
		$files.Add($_.FullName)
	}
}

$build_to = "build/Roshar.exe"

$libs_release = @(
	"-luuid.lib",
	"-lgdi32.lib",
	"-lwinmm.lib",
	"-lgdi32.lib",
	"-limgui.lib",
	"-lole32.lib",
	"-lglew32.lib",
	"-lodbc32.lib",
	"-luser32.lib",
	"-lXinput.lib",
	"-lshell32.lib",
	"-llibucrt.lib",
	"-lkernel32.lib",
	"-lwinspool.lib",
	"-lcomdlg32.lib",
	"-ladvapi32.lib",
	"-loleaut32.lib",
	"-lodbccp32.lib",
	"-lopengl32.lib"
)

"Started compiling"

& clang++.exe $files -Wall -Wextra -I"." -I"C:\Users\tackw\Code\vcpkg\installed\x86-windows-static\include" -L"C:\Users\tackw\Code\vcpkg\installed\x86-windows-static\lib" -std=c++2a -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -Xclang -flto-visibility-public-std -D SFML_STATIC -D NDEBUG -D NOMINMAX -D NATIVE -D CONSOLE $libs_release -m32 -g -D_MT -o $build_to -O2
