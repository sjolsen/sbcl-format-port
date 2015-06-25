#include "late-format.hh"
#include <tuple>
#include <cctype>


/// Implementation for PARSE-DIRECTIVE

std::size_t position (char c, string_t string, std::size_t start)
{
	for (auto pos = start; pos < length (string); ++pos)
		if (schar (string, pos) == c)
			return pos;
	return length (string);
}

std::tuple <int, std::size_t> parse_integer_unsafe (string_t string, std::size_t index)
// Requires that substring(string,index,end(string)) be non-empty and match the
// regex "^[+-]?[0-9]+"
{
	bool negative = false;
	if (schar (string, index) == '-') {
		negative = true;
		++index;
	}
	else if (schar (string, index) == '+')
		++index;

	int result = 0;
	while (index != length (string)
	       and '0' <= schar (string, index)
	       and schar (string, index) <= '9')
	{
		result *= 10;
		result += schar (string, index);
		++index;
	}

	return std::make_tuple (result, index);
}

format_directive parse_directive (string_t string, std::size_t start)
{
	auto posn    = 1 + start;
	auto params  = paramlist {};
	auto colonp  = false;
	auto atsignp = false;
	auto end     = length (string);

	auto get_char = [&] () {
		if (posn == end)
			throw format_error ()
				._complaint ("string ended before directive was found")
				._control_string (string)
				._offset (start);
		else
			return schar (string, posn);
	};

	auto check_ordering = [&] () {
		if (colonp or atsignp)
			throw format_error ()
				._complaint ("parameters found after #\\: or #\\@ modifier")
				._control_string (string)
				._offset (posn)
				._references ("ANSI CL §22.3");
	};

	while (true) {
		auto c = get_char ();

		if (('0' <= c and c <= '9') or (c == '+') or (c == '-')) {
			check_ordering ();

			int param;
			std::size_t new_posn;
			std::tie (param, new_posn) = parse_integer_unsafe (string, posn);

			params.push_back (param_t {posn}._int_param (param));
			posn = new_posn;

			switch (get_char ()) {
				case ',':
					break;
				case ':':
				case '@':
					--posn;
					break;
				default:
					goto loop_return;
			}
		}
		else if (c == 'v' or c == 'V') {
			check_ordering ();

			params.push_back (param_t {posn}._kwd_param (KWD_ARG));
			++posn;

			switch (get_char ()) {
				case ',':
					break;
				case ':':
				case '@':
					--posn;
					break;
				default:
					goto loop_return;
			}
		}
		else if (c == '#') {
			check_ordering ();

			params.push_back (param_t {posn}._kwd_param (KWD_REMAINING));
			++posn;

			switch (get_char ()) {
				case ',':
					break;
				case ':':
				case '@':
					--posn;
					break;
				default:
					goto loop_return;
			}
		}
		else if (c == '\'') {
			check_ordering ();

			++posn;
			params.push_back (param_t {posn}._char_param (get_char ()));
			++posn;

			if (get_char () != ',')
				--posn;
		}
		else if (c == ',') {
			check_ordering ();
			params.push_back (param_t {posn}._nil_param ());
		}
		else if (c == ':') {
			if (colonp)
				throw format_error ()
					._complaint ("too many colons supplied")
					._control_string (string)
					._offset (posn)
					._references ("ANSI CL §22.3");
			else
				colonp = true;
		}
		else if (c == '@') {
			if (atsignp)
				throw format_error ()
					._complaint ("too many #\\@ characters supplied")
					._control_string (string)
					._offset (posn)
					._references ("ANSI CL §22.3");
			else
				atsignp = true;
		}
		else {
			if ((schar (string, posn - 1) == ',') and
			    ((posn < 2) or (schar (string, posn - 2) != '\''))) {
				check_ordering ();
				params.push_back (param_t {posn - 1}._nil_param ());
			}
			goto loop_return;
		}
		++posn;
	}
loop_return:;
	auto c = get_char ();
	if (c == '/') {
		auto closing_slash = position ('/', string, posn + 1);
		if (closing_slash != length (string))
			posn = closing_slash;
		else
			throw format_error ()
				._complaint ("no matching closing slash")
				._control_string (string)
				._offset (posn);
	}
	return format_directive {
		string, start, posn + 1,
		static_cast <char> (std::toupper (c)),
		colonp, atsignp,
		std::move (params)
	};
}
