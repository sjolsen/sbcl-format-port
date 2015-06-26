#ifndef LATE_FORMAT_HH
#define LATE_FORMAT_HH

#include "string.hh"
#include "../disjoint-union/disjoint-union.hh"
#include <vector>
#include <memory>


/// Declarations for PARSE-DIRECTIVE

enum kwd
{
	KWD_ARG,
	KWD_REMAINING
};

struct nil_t {};
static nil_t nil;

struct param_t {
	using data_t = sjo::disjoint_union <int, kwd, char, nil_t>;

	std::size_t position;
	data_t      data;

	param_t (std::size_t posn, int data)
		: position (posn),
		  data (data_t::create <0> (data))
	{
	}

	param_t (std::size_t posn, kwd data)
		: position (posn),
		  data (data_t::create <1> (data))
	{
	}

	param_t (std::size_t posn, char data)
		: position (posn),
		  data (data_t::create <2> (data))
	{
	}

	param_t (std::size_t posn, nil_t data)
		: position (posn),
		  data (data_t::create <3> (data))
	{
	}
};

using paramlist = std::vector <param_t>;

struct format_error
{
	std::string complaint;
	// args
	std::string control_string;
	std::size_t offset;
	std::size_t second_relative = -1;
	// print_banner
	const char* references = nullptr;

	format_error (std::string complaint,
	              string_t    control_string,
	              std::size_t offset)
		: complaint (std::move (complaint)),
		  control_string (to_string (control_string)),
		  offset (offset)
	{
	}

	format_error& _second_relative (std::size_t sr) {
		second_relative = sr;
		return *this;
	}

	format_error& _references (const char* s) {
		references = s;
		return *this;
	}
};

struct format_directive
{
	string_t    string;
	std::size_t start;
	std::size_t end;
	char        character;
	bool        colonp;
	bool        atsignp;
	paramlist   params;
};

using directive_ptr = std::shared_ptr <format_directive>;

format_directive parse_directive (string_t string, std::size_t start);


/// Declarations for TOKENIZE-CONTROL-STRING

enum {
	TOK_STRING,
	TOK_DIRECTIVE
};
using token_t = sjo::disjoint_union <string_t, directive_ptr>;

using token_list = std::vector <token_t>;

token_list tokenize_control_string (string_t string);

#endif
