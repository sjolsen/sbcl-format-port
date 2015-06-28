#ifndef TYPES_DYNAMIC_ARG_ITERATOR_HH
#define TYPES_DYNAMIC_ARG_ITERATOR_HH

#include "format-arg.hh"


/// Vtable for arg iterator

struct ai_vtable_t
{
	void       (*increment) (void*);
	format_arg (*deref)     (const void*);
	bool       (*equals)    (const void*, const void*);
};

template <typename T>
struct ai_vtable_instance
{
	static const ai_vtable_t instance;
};

template <typename T>
const ai_vtable_t ai_vtable_instance <T>::instance = {
	// increment
	[] (void* p) {
		++*static_cast <T*> (p);
	},
	// deref
	[] (const void* p) {
		return format_arg (**static_cast <const T*> (p));
	},
	// equals
	[] (const void* p, const void* q) {
		return *static_cast <const T*> (p) == *static_cast <const T*> (q);
	}
};


/// Type-erased arg iterator

class arg_iterator
{
	void*              _iterator;
	const void*        _end;
	const ai_vtable_t* _vtable;

public:
	template <typename Iter>
	arg_iterator (Iter& iter, const Iter& end)
		: _iterator (&iter),
		  _end (&end),
		  _vtable (&ai_vtable_instance <Iter>::instance)
	{
	}

	arg_iterator& operator ++ () {
		_vtable->increment (_iterator);
		return *this;
	}

	format_arg operator * () const {
		return _vtable->deref (_iterator);
	}

	bool empty () const {
		return _vtable->equals (_iterator, _end);
	}
};

#endif
