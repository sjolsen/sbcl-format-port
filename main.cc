#include "late-format.hh"
#include <iostream>

void print (string_t s)
{
	std::cout << "s: \"";
	for (auto p = s.begin; p != s.end; ++p)
		std::cout << *p;
	std::cout << "\"";
}

void print (const format_directive& d)
{
	std::cout << "d: "
	          << "[" << d.start << "," << d.end << ") "
	          << "'" << d.character << "' "
	          << (d.colonp ? ':' : '_') << " "
	          << (d.atsignp ? '@' : '_') << " "
	          << "{params omitted}";
}

void print (directive_ptr p)
{
	print (*p);
}

void terpri ()
{
	std::cout << std::endl;
}

#include "../sjo/lift.hh"
#include "../sjo/compose.hh"

int main ()
{
	char s [] = "Stuff: [~{[~{~a~^, ~}]~^,~%        ~}]~%";
	auto st = string_t {std::begin (s), std::end (s) - 1};

	for (auto token : tokenize_control_string (st))
	{
		token.apply (sequence_ (LIFT (print), terpri));
	}
}
