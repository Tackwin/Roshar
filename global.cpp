#include "global.hpp"

Environment_t Environment;
std::filesystem::path Exe_Path{ std::filesystem::current_path() };

std::mutex Main_Mutex;
