#pragma once
#include <tuple>

// Kill me please what the fuck do i need to do that like that...
template <typename... Ts> struct Type_List {};

template<class> class Tuple;

template< template <typename... Args> class T, typename ...Ts>
struct Tuple<T<Ts...>> : std::tuple<Ts...> {
	using type = std::tuple<Ts...>;
	using size = std::integral_constant<size_t, sizeof...(Ts)>;
};

template<size_t ... Ns> struct sequence {};
template<size_t ... Ns> struct seq_gen;
template<size_t Head, size_t... Ns>
struct seq_gen<Head, Ns...> {
	using type = typename seq_gen<Head - 1, Head - 1, Ns...>::type;
};

template<size_t... Ns>
struct seq_gen<0, Ns...> {
	using type = sequence<Ns...>;
};

template<size_t N> using Sequence = typename seq_gen<N>::type;

template<typename T, typename F, size_t... IDX>
void for_each_(T&& t, F&& f, sequence<IDX...>) noexcept {
	(f(std::get<IDX>(t)), ...);
}
template<typename T, typename F>
void for_each(T&& t, F&& f) noexcept {
	for_each_(std::forward<T>(t), std::forward<F>(f), Sequence<std::decay_t<T>::size::value>{});
}
