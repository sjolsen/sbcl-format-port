#ifndef TYPES_KEYWORD_HH
#define TYPES_KEYWORD_HH

#include "string.hh"

// Just use string views of static strings for now. Might do full symbols later.
struct keyword
{
	string_t string;
};

#endif
