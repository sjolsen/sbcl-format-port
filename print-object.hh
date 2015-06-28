#ifndef PRINT_OBJECT_HH
#define PRINT_OBJECT_HH

#include "types/writable.hh"
#include <typeinfo>
#include <cstdio>

template <typename T>
void print_object (const T& obj, writable& w)
{
	std::string s = "#<";
	s += typeid (T).name ();
	s += ' ';
	char buf [19] = "";
	std::snprintf (buf, 19, "%p", (void*)&obj);
	s += buf;
	s += ">";
	w.write_string (std::move (s));
}

#endif
