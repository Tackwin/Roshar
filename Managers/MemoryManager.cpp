// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MemoryManager.hpp"


MemoryManager::MemoryManager() {
}
MemoryManager::~MemoryManager() {
	if (!_main_buffer)
		delete[] _main_buffer;
}

MemoryManager& MemoryManager::I() {
	static MemoryManager memory_manager;
	return memory_manager;
}

void MemoryManager::initialize_buffer(size_t size) {
	_main_size = size;
	_main_buffer = new u08[size];
	_free_memory.push_back({ _main_buffer, size });
}

size_t MemoryManager::get_buffer_size() const {
	return _main_size;
}

size_t MemoryManager::get_free_size() const {
	size_t sum = 0u;
	for (size_t i = 0u; i < _free_memory.size(); ++i) {
		const auto& place = _free_memory[i];
		sum += place.size;
	}
	return sum;
}

