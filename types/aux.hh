#ifndef TYPES_AUX_HH
#define TYPES_AUX_HH

#include <cstdint>
#include <algorithm>
#include <initializer_list>

namespace aux {


/// Numeric functions

	static inline constexpr
	bool even (std::uintmax_t n)
	{
		return n % 2 == 0;
	}

	static inline constexpr
	std::uintmax_t gcd_2 (std::uintmax_t a, std::uintmax_t b)
	{
		if (a == b) return a;
		if (a == 0) return b;
		if (b == 0) return a;

		if (even (a))
		{
			if (even (b))
				return gcd_2 (a / 2, b / 2) * 2;
			else
				return gcd_2 (a / 2, b);
		}
		else
		{
			if (even (b))
				return gcd_2 (a, b / 2);
			else {
				auto min = (b < a ? b : a);
				auto max = (b < a ? a : b);
				return gcd_2 ((max - min) / 2, min);
			}
		}
	}

	static inline constexpr
	std::uintmax_t lcm_2 (std::uintmax_t a, std::uintmax_t b)
	{
		return (a * b) / gcd_2 (a, b);
	}

	static inline constexpr
	std::uintmax_t lcm (std::initializer_list <std::uintmax_t> il)
	{
		std::uintmax_t result = 1;
		for (auto i : il)
			result = lcm_2 (result, i);
		return result;
	}

	static inline constexpr
	std::uintmax_t max (std::initializer_list <std::uintmax_t> il)
	{
		std::uintmax_t result = 0;
		for (auto i : il)
			if (i > result)
				result = i;
		return result;
	}

	
/// Type indexing

	template <std::size_t, typename, typename...>
	struct nth_type_impl;

	template <typename First, typename... Rest>
	struct nth_type_impl <0, First, Rest...>
	{
		using type = First;
	};

	template <std::size_t N, typename First, typename... Rest>
	struct nth_type_impl
	{
		using type = typename nth_type_impl <N - 1, Rest...>::type;
	};

	template <std::size_t N, typename... T>
	using nth_type = typename nth_type_impl <N, T...>::type;


/// Function type erasure

	template <typename F, typename T, typename R>
	static inline
	R (*erase_unary_lvalue_function ()) (void*, void*)
	{
		return [] (void* f, void* arg) -> R {
			return (*static_cast <F*> (f)) (*static_cast <T*> (arg));
		};
	}

	template <typename F, typename T, typename R>
	static inline
	R (*erase_unary_const_lvalue_function ()) (void*, const void*)
	{
		return [] (void* f, const void* arg) -> R {
			return (*static_cast <F*> (f)) (*static_cast <const T*> (arg));
		};
	}

	template <typename F, typename T, typename R>
	static inline
	R (*erase_unary_rvalue_function ()) (void*, void*)
	{
		return [] (void* f, void* arg) -> R {
			return (*static_cast <F*> (f)) (std::move (*static_cast <T*> (arg)));
		};
	}


/// Member function erasers

	template <typename T>
	static inline
	void copy_constructor (void* to, const void* from)
	{
		new (to) T (*static_cast <const T*> (from));
	}

	template <typename T>
	static inline
	void move_constructor (void* to, void* from)
	{
		new (to) T (std::move (*static_cast <T*> (from)));
	}

	template <typename T>
	static inline
	void destructor (void* obj)
	{
		static_cast <T*> (obj)->~T ();
	}


/// Variadic expansion

	template <typename T, typename... Pack>
	static inline
	T replicate (T t)
	{
		return std::move (t);
	}

}

#endif
