#ifndef LATE_FORMAT_HH
#define LATE_FORMAT_HH

#include "string.hh"
#include <vector>
#include <memory>


/// Declarations for PARSE-DIRECTIVE

enum kwd
{
	KWD_ARG,
	KWD_REMAINING
};

struct param_t
{
	std::size_t position;

	enum tag_t {
		int_tag,
		kwd_tag,
		char_tag,
		nil_tag
	} type;
	union {
		int  int_param;
		kwd  kwd_param;
		char char_param;
	};

	param_t& _int_param (int param) {
		type = int_tag;
		int_param = param;
		return *this;
	}

	param_t& _kwd_param (kwd param) {
		type = kwd_tag;
		kwd_param = param;
		return *this;
	}

	param_t& _char_param (char param) {
		type = char_tag;
		char_param = param;
		return *this;
	}

	param_t& _nil_param () {
		type = nil_tag;
		return *this;
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

class token_t
{
public:
	enum tag_t {
		string_tag,
		directive_tag
	};

private:
	tag_t _type;
	union {
		string_t _string;
		directive_ptr _directive;
	};

public:
	token_t (const token_t& other)
		: _type (other._type)
	{
		switch (_type) {
			case string_tag:
				new (&_string) string_t (other._string);
				break;
			case directive_tag:
				new (&_directive) directive_ptr (other._directive);
				break;
		}
	}

	token_t (string_t token)
		: _type (string_tag),
		  _string (token)
	{
	}

	token_t (directive_ptr token)
		: _type (directive_tag),
		  _directive (std::move (token))
	{
	}

	tag_t type () const {
		return _type;
	}

	string_t string () const {
		return _string;
	}

	directive_ptr directive () const {
		return _directive;
	}

	~token_t ()
	{
		switch (_type) {
			case string_tag:
				_string.~string_t ();
				break;
			case directive_tag:
				_directive.~directive_ptr ();
				break;
		}
	}
};

template <typename SFun, typename DFun>
std::common_type_t <
	std::result_of_t <SFun (string_t)>,
	std::result_of_t <DFun (directive_ptr)>
>
token_elim (const token_t& token, SFun sfun, DFun dfun)
{
	switch (token.type ()) {
		case token_t::string_tag:
			return sfun (token.string ());
		case token_t::directive_tag:
			return dfun (token.directive ());
	}
}

template <typename GFun>
std::common_type_t <
	std::result_of_t <GFun (string_t)>,
	std::result_of_t <GFun (directive_ptr)>
>
apply (GFun gfun, const token_t& token)
{
	return token_elim (token, gfun, gfun);
}

using token_list = std::vector <token_t>;

token_list tokenize_control_string (string_t string);

#endif
