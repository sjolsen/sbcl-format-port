#include "format-error.hh"
#include <ostream>

dynamic_variable <maybe <string_t> >
default_format_error_control_string = {};

dynamic_variable <maybe <std::size_t> >
default_format_error_offset = {};

std::ostream& print_format_error (std::ostream& os, const format_error& err)
{
	if (err.print_banner)
		os << "error in FORMAT: ";
	os << err.complaint << '\n';

	if (err.control_string) {
		os << "  " << *err.control_string << '\n'
		   << "  ";
		for (std::size_t i = 0; i < err.offset.value_or (0); ++i)
			os << ' ';
		os << '^';
		if (err.second_relative) {
			for (std::size_t i = 0; i < *err.second_relative; ++i)
				os << ' ';
			os << '^';
		}
	}

	return os;
}
