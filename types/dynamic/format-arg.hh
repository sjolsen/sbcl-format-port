#ifndef TYPES_DYNAMIC_FORMAT_ARG_HH
#define TYPES_DYNAMIC_FORMAT_ARG_HH

#include "vtable.hh"

class format_arg
{
protected:
	const void*     _arg;
	const vtable_t* _vtable;

public:
	template <typename T>
	format_arg (const T& obj)
		: _arg (&obj),
		  _vtable (&vtable_instance <T>::instance)
	{
	}

	void print (writable& w) const {
		_vtable->print_object (_arg, w);
	}
};

#endif
