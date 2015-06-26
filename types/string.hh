#ifndef TYPES_STRING_HH
#define TYPES_STRING_HH

#include "shared-string-view.hh"
#include <algorithm>

using string_t = shared_string_view;

static inline
std::size_t length (string_t string)
{
	return string.length ();
}

static inline
std::size_t position (char c, string_t string, std::size_t start)
{
	auto end = std::find (string.begin () + start, string.end (), c);
	return end - string.begin ();
}

#endif
