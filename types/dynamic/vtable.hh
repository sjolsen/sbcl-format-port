#ifndef TYPES_DYNAMIC_VTABLE_HH
#define TYPES_DYNAMIC_VTABLE_HH

#include "traits.hh"
#include "../../print-object.hh"


/// Total vtable

struct vtable_t
{
	void (*print_object) (const void*, writable&);
};

template <typename T>
struct vtable_instance
{
	static const vtable_t instance;
};

template <typename T>
const vtable_t vtable_instance <T>::instance = {
	// print_object
	[] (const void* p, writable& w) {
		// using ns::print_object;
		print_object (*static_cast <const T*> (p), w);
	}
};

#endif
