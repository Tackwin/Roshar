cmake_minimum_required(VERSION 3.0.0)
project(Roshar VERSION 0.1.0)

include(CTest)
enable_testing()


file(GLOB_RECURSE ALL_SRC "*.hpp" "*.cpp")

add_executable(Roshar ${ALL_SRC})

set_property(TARGET Roshar PROPERTY CXX_STANDARD 20)

target_include_directories(Roshar PRIVATE
	"C:/Users/tackw/Code/vcpkg/installed/x64-windows-static/include"
	"C:/Users/tackw/Code/vcpkg/installed/x86-windows-static/include"
	"."
)

target_include_directories(Roshar PRIVATE
	"C:/Users/tackw/Code/vcpkg/installed/x64-windows-static/include"
	"C:/Users/tackw/Code/vcpkg/installed/x86-windows-static/include"
	"."
)

add_definitions(/FI"xstd.hpp")
add_definitions(/FI"global.hpp")
add_definitions(/FI"macros.hpp")
add_definitions(/FI"constexpr.hpp")

add_compile_definitions(NDEBUG)
add_compile_definitions(CONSOLE)
add_compile_definitions(NATIVE)
add_compile_definitions(NOMINMAX)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
