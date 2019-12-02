#include "OS/Process.hpp"
#include "Windows.h"

std::size_t get_process_id() noexcept {
	return (std::size_t)GetCurrentProcessId();
}

std::size_t get_thread_id() noexcept {
	return (std::size_t)GetCurrentThreadId();
}
