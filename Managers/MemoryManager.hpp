#pragma once
#include <algorithm>
#include <cstdint>
#include <utility>
#include <utility>
#include <vector>
#include <memory>
#include <cmath>

// trying the singleton thing
// and snake_case sssssssss
class MemoryManager {
	using u08 = std::uint8_t;

	struct mem_place {
		u08* location;
		std::size_t size;
	};
public:

	template<typename T>
	using unique_ptr = std::unique_ptr<T>;


	template<typename T>
	struct Allocator {
		using value_type = T;

		Allocator() = default;
		template<class U> constexpr Allocator(const Allocator<U>&) noexcept {}

		T& operator*() const { return *_t; };
		T& operator*() { return *_t; };

		T* operator->() const { return _t.get(); }
		T* operator->() { return _t.get(); }

		T* allocate(std::size_t n) {
			return MM::allocate(n)
		}

		void deallocate(T* p, std::size_t n) {
			MM::deallocate(p, n);
		}
	};


	static MemoryManager& I();
	/*
	template<typename T, class... Args>
	static std::shared_ptr<T> make_shared(Args&&... args) {
		return std::shared_ptr<T>(std::forward<Args>(args)...);
		const auto& ptr =
			MemoryManager::I().construct<T>(std::forward<Args>(args)...);

		return std::shared_ptr<T>(
			ptr,
			std::bind(
				&MemoryManager::deallocate<T>,
				&MemoryManager::I(),
				std::placeholders::_1,
				sizeof(T)
			)
		);
	}

	template<typename T>
	static std::shared_ptr<T> make_sshared(T* ptr) {
		return std::shared_ptr<T>(ptr);
		return std::shared_ptr<T>(
			ptr,
			std::bind(
				&MemoryManager::deallocate<T>,
				&MemoryManager::I(),
				std::placeholders::_1,
				sizeof(T)
			)
		);
	}

	template<typename T>
	static unique_ptr<T> make_unisque(T* ptr) {
		return std::unique_ptr<T>(ptr);
		//return unique_ptr<T>(ptr, [](T* ptr) {MemoryManager::I().deallocate(ptr); });
	}

	template<typename T, class... Args>
	static unique_ptr<T> make_unique(Args&&... args) {
		return std::unique_ptr<T>(std::forward<Args>(args)...);
		T* ptr = MemoryManager::I().construct<T>(std::forward<Args>(args)...);

		return unique_ptr<T>(
			ptr,
			[](T* ptr) {
			MemoryManager::I().deallocate(ptr);
		}
		);
	}
	template<typename T, typename U>
	static unique_ptr<T> cast_unique(U* ptr) {
		return unique_ptr<T>(
			static_cast<T*>(ptr),
			[](T* ptr) {
			MemoryManager::I().deallocate(ptr);
		}
		);
	}*/


	size_t get_buffer_size() const;
	size_t get_free_size() const;
	void initialize_buffer(size_t size);

	template<typename T>
	T* allocate(std::size_t n) {

		//testing if the memory place is big enough to hold T
		for (size_t i = 0u; i < _free_memory.size(); ++i) {

			mem_place free_place = _free_memory[i];
			mem_place free_place_aligned = _free_memory[i];

			void* free_place_aligned_location =
				(void*)free_place_aligned.location;

			if (!std::align(
				alignof(T),
				sizeof(T) * n,
				free_place_aligned_location,
				free_place_aligned.size
			))
				// if we can't properly align the memory place it's no use
				continue;

			free_place_aligned.location = (u08*)free_place_aligned_location;

			// if it's big enough to hold T then let's use it !
			if (free_place.size >= sizeof(T) * n) {

				//if the aligned ptr is not the normal one, then there's
				// a new memory place at the begining
				if (free_place.location != free_place_aligned.location) {
					_free_memory.push_back({
						free_place.location,
						(std::size_t)
						((u08*)free_place_aligned_location - free_place.location)
						});
				}

				// so if it isn't _exactly_ the size there's a new place 
				// at the end
				if (free_place.size != sizeof(T) * n)
					_free_memory.push_back({
					free_place.location + sizeof(T) * n,
					free_place.size - sizeof(T) * n
						});

				// we delete the memory place that we use
				_free_memory.erase(_free_memory.begin() + i);

				std::sort(
					_free_memory.begin(),
					_free_memory.end(),
					[](const mem_place& A, const mem_place& B) -> bool {
					return A.location < B.location;
				}
				);

				return reinterpret_cast<T*>(free_place.location);
			}
		}

		throw std::runtime_error("Not enough place");
	}

	template<typename T, class... Args>
	T* construct(Args&&... args) {
		return new(allocate<T>(1)) T(std::forward<Args>(args)...);
	}

	template<typename T>
	void deallocate(T* ptr, size_t size = 1) {
		ptr->~T();

		bool flag = true;

		// We will create a new memory_place, i'll go through the loop 
		// to merge all consecutive memory_place
		for (size_t i = 0u; i < _free_memory.size() && flag; ++i) {

			mem_place free_place = _free_memory[i];

			// if the memory place is just succeding another, we merge them
			if (free_place.location + free_place.size == (u08*)ptr) {
				_free_memory[i].size += sizeof(T) * size;

				// maybe with that bigger size, the next mem_place touch me
				if (i + 1 < _free_memory.size()) {
					if (
						_free_memory[i + 1].location == free_place.location +
						free_place.size
						) {
						// if that''s the case, i need to grow even more
						_free_memory[i].size += _free_memory[i + 1].size;
						_free_memory.erase(std::begin(_free_memory) + i + 1);
					}
				}

				flag = false;
			}

			// if the memory place is just preceding another, we merge them
			if (
				(std::size_t)free_place.location > sizeof(T) * size &&
				free_place.location - sizeof(T) * size == (u08*)ptr
				) {
				_free_memory[i].location -= sizeof(T) * size;
				_free_memory[i].size += sizeof(T) * size;
				flag = false;
			}
		}

		if (flag) { // else we just push a new memory place
			_free_memory.push_back({ (u08*)ptr, sizeof(T) * size });
		}

		std::sort(
			_free_memory.begin(),
			_free_memory.end(),
			[](const mem_place& A, const mem_place& B) -> bool {
			return A.location < B.location;
		}
		);
	}

private:
	MemoryManager();
	~MemoryManager();

	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	size_t _main_size = 0u;
	u08* _main_buffer = nullptr;

	std::vector<mem_place> _free_memory;
};

// Optional
using MM = MemoryManager;