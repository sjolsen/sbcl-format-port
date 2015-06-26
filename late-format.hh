#ifndef LATE_FORMAT_HH
#define LATE_FORMAT_HH

#include "types/string.hh"
#include "types/disjoint-union.hh"
#include "types/maybe.hh"
#include "types/keyword.hh"
#include "types/null.hh"
#include <vector>
#include <memory>

// Error type from Lisp. May be able to redesign.
struct format_error
{
	std::string         complaint;
	std::string         control_string;
	std::size_t         offset;
	maybe <std::size_t> second_relative = {};
	maybe <string_view> references      = {};

	format_error (std::string complaint,
	              string_view control_string,
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

	format_error& _references (string_view s) {
		references = s;
		return *this;
	}
};


/// Declarations for PARSE-DIRECTIVE

struct param_t
{
	using data_t = disjoint_union <int, keyword, char, null>;
	enum : std::size_t {
		DATA_INT  = 0,
		DATA_KWD  = 1,
		DATA_CHAR = 2,
		DATA_NULL = 3
	};

	std::size_t position;
	data_t      data;

	param_t (std::size_t posn, int data)
		: position (posn),
		  data (data_t::create <DATA_INT> (data))
	{
	}

	param_t (std::size_t posn, keyword data)
		: position (posn),
		  data (data_t::create <DATA_KWD> (data))
	{
	}

	param_t (std::size_t posn, char data)
		: position (posn),
		  data (data_t::create <DATA_CHAR> (data))
	{
	}

	param_t (std::size_t posn, null data)
		: position (posn),
		  data (data_t::create <DATA_NULL> (data))
	{
	}
};

using paramlist = std::vector <param_t>;

struct format_directive
{
	string_view string;
	std::size_t start;
	std::size_t end;
	char        character;
	bool        colonp;
	bool        atsignp;
	paramlist   params;
};

using directive_ptr = std::shared_ptr <format_directive>;

format_directive parse_directive (string_view string, std::size_t start);


/// Declarations for TOKENIZE-CONTROL-STRING

struct token_t
	: disjoint_union <string_view, directive_ptr>
{
	using type = disjoint_union <string_view, directive_ptr>;
	enum : std::size_t {
		TOKEN_STRING    = 0,
		TOKEN_DIRECTIVE = 1
	};

	token_t (string_view data)
		: type (type::create <TOKEN_STRING> (data))
	{
	}

	token_t (directive_ptr data)
		: type (type::create <TOKEN_DIRECTIVE> (data))
	{
	}
};

using token_list = std::vector <token_t>;

token_list tokenize_control_string (string_view string);

#endif
