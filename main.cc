#include "late-format.hh"
#include <iostream>

void print (string_t s)
{
	std::cout << "s: \"" << s << "\"";
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

void print (const directive_ptr& p)
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
try
{
	auto s = string_t {"Stuff: [~{[~{~a~^, ~}]~^,~%        ~}]~%"};
	std::cout << s << std::endl;

	for (const auto& token : tokenize_control_string (s))
	{
		token.apply (sequence_ (LIFT (print), terpri));
	}

	tokenize_control_string ("~>");
}
catch (const format_error& err)
{
	print_format_error (std::cerr, err) << std::endl;
}
