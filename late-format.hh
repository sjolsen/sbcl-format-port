#ifndef LATE_FORMAT_HH
#define LATE_FORMAT_HH

#include "string.hh"
#include <vector>

enum kwd {
	KWD_ARG,
	KWD_REMAINING
};

struct param_t {
	std::size_t position;

	enum tag_t {
		int_t,
		kwd_t,
		char_t,
		nil_t
	} type;
	union {
		int  int_param;
		kwd  kwd_param;
		char char_param;
	};

	param_t& _int_param (int param) {
		type = int_t;
		int_param = param;
		return *this;
	}

	param_t& _kwd_param (kwd param) {
		type = kwd_t;
		kwd_param = param;
		return *this;
	}

	param_t& _char_param (char param) {
		type = char_t;
		char_param = param;
		return *this;
	}

	param_t& _nil_param () {
		type = nil_t;
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
	// second_relative
	// print_banner
	const char* references;

	format_error& _complaint (std::string s) {
		complaint = std::move (s);
		return *this;
	}

	format_error& _control_string (string_t s) {
		control_string = to_string (s);
		return *this;
	}

	format_error& _offset (std::size_t o) {
		offset = o;
		return *this;
	}

	format_error& _references (const char* s) {
		references = s;
		return *this;
	}
};

struct format_directive {
	string_t    string;
	std::size_t start;
	std::size_t end;
	char        character;
	bool        colonp;
	bool        atsignp;
	paramlist   params;
};

format_directive parse_directive (string_t string, std::size_t start);

#endif
