#ifndef LATE_FORMAT_HH
#define LATE_FORMAT_HH

#include "format-error.hh"
#include "types/string.hh"
#include "types/disjoint-union.hh"
#include "types/maybe.hh"
#include "types/keyword.hh"
#include "types/null.hh"
#include <vector>
#include <memory>


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

struct token_t
	: disjoint_union <string_t, directive_ptr>
{
	using type = disjoint_union <string_t, directive_ptr>;
	enum : std::size_t {
		TOKEN_STRING    = 0,
		TOKEN_DIRECTIVE = 1
	};

	token_t (string_t data)
		: type (type::create <TOKEN_STRING> (data))
	{
	}

	token_t (directive_ptr data)
		: type (type::create <TOKEN_DIRECTIVE> (data))
	{
	}
};

using token_list = std::vector <token_t>;

token_list tokenize_control_string (string_t string);

#endif
