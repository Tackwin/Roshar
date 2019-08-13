#pragma once

#include <type_traits>
#include <string>
#include <vector>
#include <atomic>
#include "Time.hpp"

namespace xstd {
	template<typename T>
	void type_id() {};

	using type_id_t = void(*)();


	template<class T>
	struct is_vector {
		using type = T;
		using contained = T;
		constexpr static bool value = false;
	};

	template<class T>
	struct is_vector<std::vector<T>> {
		using type = std::vector<T>;
		using contained_t = T;
		constexpr static bool value = true;
	};

	template< typename T>
	inline constexpr bool is_vector_v = is_vector<T>::value;

	template< typename T>
	using is_vector_contained = typename is_vector<T>::contained_t;

	template< typename T>
	using is_vector_t = typename is_vector<T>::type;

	inline std::uint64_t uuid() noexcept {
		static std::atomic<std::uint16_t> counter = 0;
		return (nanoseconds() << 16) | (counter++);
	}

	template<typename C, typename P>
	[[nodiscard]] C filter(const C& c, const P& p) noexcept {
		C result;
		auto it = std::back_inserter(result);
		for (auto& x : c) if (p(x)) *it++ = x;
		return result;
	}

	struct error_t {
		std::size_t code{ 0 };
		std::string desc{ "" };
	};

	template<typename T, typename E>
	struct expected {
		expected(T t) noexcept : t(std::move(t)), is_error(false) {}
		expected(E e) noexcept : e(std::move(e)), is_error(true) {}
		~expected() noexcept {
			if (is_error) {
				if constexpr (std::is_class_v<E>) e.~E();
			}
			else if constexpr (std::is_class_v<T>) {
				t.~T();
			}
		}
		operator bool() noexcept { return !is_error; }
		explicit operator T() noexcept { return t; }
		explicit operator E() noexcept { return e; }
		T& value() noexcept { return t; }
		E& error() noexcept { return e; }

		T& operator->() noexcept { return t; }
		T& operator*() noexcept { return t; }
	private:
		union {
			E e;
			T t;
		};
		bool is_error{ false };
	};

	template<typename T>
	using std_expected = expected<T, size_t>;

	constexpr inline std::size_t hash(std::size_t h, const char* str) noexcept {
		return !*str ? 0 : hash((h << 5) + h + *str, str + 1);
	}
	constexpr inline std::size_t hash(const char* str) noexcept {
		return !str ? 0 : hash(5381, str);
	}

	template<typename T>
	constexpr inline std::size_t hash_combine(std::size_t seed, const T& v) noexcept {
		std::hash<T> h;
		seed ^= h(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
	constexpr inline std::size_t hash_combine(std::size_t a, size_t b) noexcept {
#pragma warning(push)
#pragma warning(disable: 4307)
		return a + 0x9e3779b9u + (b << 6) + (b >> 2);
#pragma warning(pop)
	}

}
