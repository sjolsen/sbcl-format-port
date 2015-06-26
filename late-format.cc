#include "late-format.hh"
#include <tuple>
#include <cctype>


/// Implementation for PARSE-DIRECTIVE

static
std::tuple <int, std::size_t> parse_integer_unsafe (string_view string, std::size_t index)
// Requires that substring(string,index,end(string)) be non-empty and match the
// regex "^[+-]?[0-9]+"
{
	bool negative = false;
	if (string [index] == '-') {
		negative = true;
		++index;
	}
	else if (string [index] == '+')
		++index;

	int result = 0;
	while (index != length (string)
	       and '0' <= string [index]
	       and string [index] <= '9')
	{
		result *= 10;
		result += string [index];
		++index;
	}

	if (negative)
		result = -result;
	return std::make_tuple (result, index);
}

format_directive parse_directive (string_view string, std::size_t start)
{
	auto posn    = start + static_cast <std::size_t> (1);
	auto params  = paramlist {};
	auto colonp  = false;
	auto atsignp = false;
	auto end     = length (string);

	auto get_char = [&] () {
		if (posn == end)
			throw format_error ("string ended before directive was found",
			                    string,
			                    start);
		else
			return string [posn];
	};

	auto check_ordering = [&] () {
		if (colonp or atsignp)
			throw format_error ("parameters found after #\\: or #\\@ modifier",
			                    string,
			                    posn)
				._references ("ANSI CL §22.3");
	};

	while (true) {
		auto c = get_char ();

		if (('0' <= c and c <= '9') or (c == '+') or (c == '-')) {
			check_ordering ();

			int param;
			std::size_t new_posn;
			std::tie (param, new_posn) = parse_integer_unsafe (string, posn);

			params.push_back (param_t {posn, param});
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

			params.push_back (param_t {posn, keyword {"ARG"}});
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

			params.push_back (param_t {posn, keyword {"REMAINING"}});
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
			params.push_back (param_t {posn, get_char ()});
			++posn;

			if (get_char () != ',')
				--posn;
		}
		else if (c == ',') {
			check_ordering ();
			params.push_back (param_t {posn, nil});
		}
		else if (c == ':') {
			if (colonp)
				throw format_error ("too many colons supplied",
				                    string,
				                    posn)
					._references ("ANSI CL §22.3");
			else
				colonp = true;
		}
		else if (c == '@') {
			if (atsignp)
				throw format_error ("too many #\\@ characters supplied",
				                    string,
				                    posn)
					._references ("ANSI CL §22.3");
			else
				atsignp = true;
		}
		else {
			if ((string [posn - 1] == ',') and
			    ((posn < 2) or (string [posn - 2] != '\''))) {
				check_ordering ();
				params.push_back (param_t {posn - 1, nil});
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
			throw format_error ("no matching closing slash",
			                    string,
			                    posn);
	}

	return format_directive {
		string, start, posn + 1,
		static_cast <char> (std::toupper (c)),
		colonp, atsignp,
		std::move (params)
	};
}


/// Implementation for TOKENIZE-CONTROL-STRING

token_list tokenize_control_string (string_view string)
{
	using directive_ptr = std::shared_ptr <format_directive>;

	auto index                   = static_cast <std::size_t> (0);
	auto end                     = length (string);
	auto result                  = token_list {};
	auto block                   = std::vector <directive_ptr> {};
	auto pprint                  = directive_ptr {};
	auto semicolon               = directive_ptr {};
	auto justification_semicolon = directive_ptr {};

	while (true) {
		auto next_directive = position ('~', string, index);

		if (next_directive > index)
			result.push_back (token_t {subseq (string, index, next_directive)});
		if (next_directive == end)
			goto loop_return;

		auto directive = std::make_shared <format_directive> (parse_directive (string, next_directive));
		auto c         = directive->character;

		// this processing is required by CLHS 22.3.5.2
		if (c == '<') {
			block.push_back (directive);
		}
		else if (!block.empty () and (c == ';') and directive->colonp) {
			semicolon = directive;
		}
		else if (c == '>') {
			if (block.empty ())
				throw format_error ("~> without a matching ~<",
				                    string,
				                    next_directive);

			if (directive->colonp) {
				if (!pprint)
					pprint = block.back ();
				semicolon = nullptr;
			}
			else if (semicolon) {
				if (!justification_semicolon)
					justification_semicolon = semicolon;
			}

			block.pop_back ();
		}
		else if (block.empty ()) {
			// block cases are handled by the #\< expander/interpreter
			switch (c) {
				case 'W':
				case 'I':
				case '_':
					if (!pprint)
						pprint = directive;
					break;
				default:
					if (directive->colonp and !pprint)
						pprint = directive;
					break;
			}
		}

		result.push_back (token_t {directive});
		index = directive->end;
	}
loop_return:;
	if (pprint and justification_semicolon) {
		auto pprint_offset        = pprint->end                  - static_cast <std::size_t> (1);
		auto justification_offset = justification_semicolon->end - static_cast <std::size_t> (1);

		throw format_error ("misuse of justification and pprint directives",
		                    string,
		                    std::min (pprint_offset, justification_offset))
			._second_relative (std::max (pprint_offset, justification_offset)
			                   - std::min (pprint_offset, justification_offset)
			                   - static_cast <std::size_t> (1))
			._references ("ANSI CL §22.3.5.2");
	}

	return result;
}
