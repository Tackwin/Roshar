#include "global.hpp"

Environment_t Environment;
std::filesystem::path Exe_Path{ std::filesystem::current_path() };

std::mutex Main_Mutex;

float wheel_scroll;

bool application_running{true};

void* platform::handle_window{ nullptr };
void* platform::handle_dc_window{ nullptr };
void* platform::main_opengl_context{ nullptr };
void* platform::asset_opengl_context{ nullptr };
