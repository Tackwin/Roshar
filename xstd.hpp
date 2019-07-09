#pragma once

#include <type_traits>

namespace xstd {
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
				std::string s;
				if constexpr (std::is_class_v<E>) e.~E();
			}
			else if constexpr (std::is_class_v<T>) {
				t.~T();
			}
		}
		operator bool() noexcept { return is_error; }
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
	using std_expected = expected<T, const char* const>;
}
