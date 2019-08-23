$files = New-Object System.Collections.Generic.List[System.Object]

Get-ChildItem -Recurse -Path . -File | ForEach-Object {
	if ($_.Extension -eq ".cpp") {
		$files.Add($_.FullName)
	}
}

$build_to = "build/Roshar.exe"

$libs_debug = @("-lsfml-graphics-s-d.lib", "-lsfml-window-s-d.lib", "-lsfml-system-s-d.lib", "-lopengl32.lib", "-lwinmm.lib", "-lgdi32.lib", "-limguid.lib", "-lkernel32.lib", "-luser32.lib", "-lgdi32.lib", "-lwinspool.lib", "-lcomdlg32.lib", "-ladvapi32.lib", "-lshell32.lib", "-lole32.lib", "-loleaut32.lib", "-luuid.lib", "-lodbc32.lib", "-lodbccp32.lib", "-llibucrtd.lib")

$libs_release = @("-lsfml-graphics-s.lib", "-lsfml-window-s.lib", "-lsfml-system-s.lib", "-lopengl32.lib", "-lwinmm.lib", "-lgdi32.lib", "-limgui.lib", "-lkernel32.lib", "-luser32.lib", "-lgdi32.lib", "-lwinspool.lib", "-lcomdlg32.lib", "-ladvapi32.lib", "-lshell32.lib", "-lole32.lib", "-loleaut32.lib", "-luuid.lib", "-lodbc32.lib", "-lodbccp32.lib", "-llibucrt.lib")

& clang++.exe $files -I"." -I"C:\Users\tackw\Code\vcpkg\installed\x86-windows-static\include" -L"C:\Users\tackw\Code\vcpkg\installed\x86-windows-static\lib" -std=c++2a -include"global.hpp" -include"xstd.hpp" -include"constexpr.hpp" -include"macros.hpp" -Xclang -flto-visibility-public-std -D SFML_STATIC $libs_release -m32 -g -D_NDEBUG -D_MT -o $build_to -O2

