#ifndef FORMAT_ERROR_HH
#define FORMAT_ERROR_HH

#include "types/string.hh"
#include "types/disjoint-union.hh"
#include "types/maybe.hh"
#include "magic/dynamic-bind.hh"
#include <iosfwd>

extern dynamic_variable <maybe <string_t> >
default_format_error_control_string;

extern dynamic_variable <maybe <std::size_t> >
default_format_error_offset;

// Error type from Lisp. May be able to redesign.
struct format_error
{
	string_t            complaint;
	maybe <string_t>    control_string;
	maybe <std::size_t> offset;
	maybe <std::size_t> second_relative = {};
	bool                print_banner    = true;
	maybe <string_t>    references      = {};

	format_error (string_t complaint)
		: complaint      (std::move (complaint)),
		  control_string (default_format_error_control_string),
		  offset         (default_format_error_offset)
	{
	}

	format_error () = delete;
	format_error (const format_error&) = delete;
	format_error (format_error&&) = default;
	format_error& operator = (const format_error&) = delete;
	format_error& operator = (format_error&&) = default;

	format_error&& _control_string (string_t s) && {
		control_string = std::move (s);
		return std::move (*this);
	}

	format_error&& _offset (std::size_t o) && {
		offset = o;
		return std::move (*this);
	}

	format_error&& _second_relative (std::size_t sr) && {
		second_relative = sr;
		return std::move (*this);
	}

	format_error&& _print_banner (bool b) && {
		print_banner = b;
		return std::move (*this);
	}

	format_error&& _references (string_t s) && {
		references = s;
		return std::move (*this);
	}
};

std::ostream& print_format_error (std::ostream& os, const format_error& err);

#endif
