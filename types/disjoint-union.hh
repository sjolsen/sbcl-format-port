#ifndef TYPES_DISJOINT_UNION_HH
#define TYPES_DISJOINT_UNION_HH

#include "aux.hh"
#include <type_traits>
#include <utility>
#include <stdexcept>

struct disjoint_union_access_error
	: std::logic_error
{
	disjoint_union_access_error ()
		: std::logic_error ("Accessed a disjoint union with an invalid index")
	{
	}
};

template <typename... T>
class disjoint_union
{
	using storage_t = std::aligned_storage_t <
		aux::max ({sizeof  (T)...}),
		aux::lcm ({alignof (T)...})
		>;

	storage_t   _storage;
	std::size_t _index;

	static void (*copy_constructors [sizeof... (T)]) (void*, const void*);
	static void (*move_constructors [sizeof... (T)]) (void*, void*);
	static void (*destructors [sizeof... (T)]) (void*);

	disjoint_union (std::size_t index)
		: _index (index)
	{
	}

public:
	template <std::size_t index>
	static std::enable_if_t <index < sizeof... (T), disjoint_union>
	create ()
	{
		disjoint_union result (index);
		new (&result._storage) aux::nth_type <index, T...> ();
		return result;
	}

		template <std::size_t index>
		static std::enable_if_t <index < sizeof... (T), disjoint_union>
		create (const aux::nth_type <index, T...>& obj)
	{
		disjoint_union result (index);
		new (&result._storage) aux::nth_type <index, T...> (obj);
		return result;
	}

			template <std::size_t index>
			static std::enable_if_t <index < sizeof... (T), disjoint_union>
			create (aux::nth_type <index, T...>&& obj)
	{
		disjoint_union result (index);
		new (&result._storage) aux::nth_type <index, T...> (std::move (obj));
		return result;
	}

	disjoint_union () = delete;

	disjoint_union (const disjoint_union& other)
		: _index (other._index)
	{
		copy_constructors [_index] (&_storage, &other._storage);
	}

	disjoint_union (disjoint_union&& other)
		: _index (other._index)
	{
		move_constructors [_index] (&_storage, &other._storage);
	}

	~disjoint_union ()
	{
		destructors [_index] (&_storage);
	}

	std::size_t index () const
	{
		return index;
	}

	template <std::size_t index>
	aux::nth_type <index, T...>& get_unsafe ()
	{
		return *static_cast <aux::nth_type <index, T...>*> (&_storage);
	}

	template <std::size_t index>
	const aux::nth_type <index, T...>& get_unsafe () const
	{
		return *static_cast <const aux::nth_type <index, T...>*> (&_storage);
	}

	template <std::size_t index>
	aux::nth_type <index, T...>& get ()
	{
		if (_index != index)
			throw disjoint_union_access_error ();
		return get_unsafe ();
	}

	template <std::size_t index>
	const aux::nth_type <index, T...>& get () const
	{
		if (_index != index)
			throw disjoint_union_access_error ();
		return get_unsafe ();
	}

	template <typename... F>
	std::common_type_t <std::result_of_t <F (T&)>...>
	elim (F... f) &
	{
		using R = std::common_type_t <std::result_of_t <F (T&)>...>;

		R (*callers [sizeof... (T)]) (void*, void*) = {
			aux::erase_unary_lvalue_function <F, T, R> ()...
		};
		void* fobjs [sizeof... (T)] = {&f...};

		return (*callers [_index]) (fobjs [_index], &_storage);
	}

	template <typename... F>
	std::common_type_t <std::result_of_t <F (const T&)>...>
	elim (F... f) const &
	{
		using R = std::common_type_t <std::result_of_t <F (const T&)>...>;

		R (*callers [sizeof... (T)]) (void*, const void*) = {
			aux::erase_unary_const_lvalue_function <F, T, R> ()...
		};
		void* fobjs [sizeof... (T)] = {&f...};

		return (*callers [_index]) (fobjs [_index], &_storage);
	}

	template <typename... F>
	std::common_type_t <std::result_of_t <F (T&&)>...>
	elim (F... f) &&
	{
		using R = std::common_type_t <std::result_of_t <F (T&&)>...>;

		R (*callers [sizeof... (T)]) (void*, void*) = {
			aux::erase_unary_rvalue_function <F, T, R> ()...
		};
		void* fobjs [sizeof... (T)] = {&f...};

		return (*callers [_index]) (fobjs [_index], &_storage);
	}

	template <typename G>
	std::common_type_t <std::result_of_t <G (T&)>...>
	apply (G g) &
	{
		return elim (aux::replicate <G, T> (g)...);
	}

	template <typename G>
	std::common_type_t <std::result_of_t <G (const T&)>...>
	apply (G g) const &
	{
		return elim (aux::replicate <G, T> (g)...);
	}

	template <typename G>
	std::common_type_t <std::result_of_t <G (T&&)>...>
	apply (G g) &&
	{
		return std::move (*this).elim (aux::replicate <G, T> (g)...);
	}
};

template <typename... T>
void (*disjoint_union <T...>::copy_constructors [sizeof... (T)]) (void*, const void*) = {
	&aux::copy_constructor <T>...
};

template <typename... T>
void (*disjoint_union <T...>::move_constructors [sizeof... (T)]) (void*, void*) = {
	&aux::move_constructor <T>...
};

template <typename... T>
void (*disjoint_union <T...>::destructors [sizeof... (T)]) (void*) = {
	&aux::destructor <T>...
};

#endif
