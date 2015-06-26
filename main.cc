#include "late-format.hh"
#include <iostream>

void print (string_view s)
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
	auto s = string_view {"Stuff: [~{[~{~a~^, ~}]~^,~%        ~}]~%"};

	for (auto token : tokenize_control_string (s))
	{
		token.apply (sequence_ (LIFT (print), terpri));
	}
}
