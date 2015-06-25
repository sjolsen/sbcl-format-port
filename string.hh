#ifndef STRING_HH
#define STRING_HH

#include <string>

struct string_t
{
	const char* begin;
	const char* end;
};

static inline
std::size_t length (string_t string)
{
	return string.end - string.begin;
}

static inline
char schar (string_t string, std::size_t index)
{
	return string.begin [index];
}

static inline
std::string to_string (string_t string)
{
	return std::string (string.begin, length (string));
}

#endif
