#ifndef MAGIC_DYNAMIC_BIND_HH
#define MAGIC_DYNAMIC_BIND_HH

#include <utility>

template <typename T>
class dynamic_binding;

template <typename T>
class dynamic_variable
{
	T  _initial_object;
	T* _current_binding = &_initial_object;

protected:
	friend class dynamic_binding <T>;

	T* _rebind (T* new_binding)
	{
		T* old_binding = _current_binding;
		_current_binding = new_binding;
		return old_binding;
	}

public:
	using value_type = T;

	template <typename... Args>
	dynamic_variable (Args&&... args)
		: _initial_object (std::forward <Args> (args)...)
	{
	}

	T& get ()
	{
		return *_current_binding;
	}

	operator T& ()
	{
		return get ();
	}
};

template <typename T>
class dynamic_variable <T&>
{
	T* _current_binding;

protected:
	friend class dynamic_binding <T&>;

	T* _rebind (T* new_binding)
	{
		T* old_binding = _current_binding;
		_current_binding = new_binding;
		return old_binding;
	}

public:
	using value_type = T&;

	dynamic_variable (T& initial_binding)
		: _current_binding (&initial_binding)
	{
	}

	template <typename U>
	dynamic_variable (U&&) = delete;

	T& get ()
	{
		return *_current_binding;
	}

	operator T& ()
	{
		return *_current_binding;
	}
};

template <typename T>
class dynamic_binding
{
	T  _local_object;
	T* _previous_binding;
	dynamic_variable <T>& _variable;

public:
	template <typename... Args>
	dynamic_binding (dynamic_variable <T>& variable, Args&&... args)
		: _local_object (std::forward <Args> (args)...),
		  _variable (variable)
	{
		_previous_binding = _variable._rebind (&_local_object);
	}

	~dynamic_binding ()
	{
		_variable._rebind (_previous_binding);
	}
};

template <typename T>
class dynamic_binding <T&>
{
	T* _previous_binding;
	dynamic_variable <T&>& _variable;

public:
	dynamic_binding (dynamic_variable <T&>& variable, T& local_binding)
		: _variable (variable)
	{
		_previous_binding = _variable._rebind (&local_binding);
	}

	template <typename U>
	dynamic_binding (U&&) = delete;

	~dynamic_binding ()
	{
		_variable._rebind (_previous_binding);
	}
};

#define MAGIC_CATENATE_IMPL(FOO, BAR) FOO##BAR
#define MAGIC_CATENATE(FOO, BAR) MAGIC_CATENATE_IMPL (FOO, BAR)
#define MAGIC_GENSYM(S) MAGIC_CATENATE (S##_, __COUNTER__)
#define DYNAMIC_BIND(NAME, ...) dynamic_binding <typename decltype (NAME)::value_type> GENSYM (local_binding) (NAME, ##__VA_ARGS__)

#endif
