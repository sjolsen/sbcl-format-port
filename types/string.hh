#ifndef TYPES_STRING_HH
#define TYPES_STRING_HH

#include <experimental/string_view>

using string_view = std::experimental::string_view;

static inline
std::size_t length (string_view string)
{
	return string.length ();
}

static inline
std::string to_string (string_view string)
{
	return string.to_string ();
}

static inline
string_view subseq (string_view string, std::size_t begin, std::size_t end)
{
	return string.substr (begin, end - begin);
}

#endif
