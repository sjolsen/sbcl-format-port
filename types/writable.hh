#ifndef TYPES_WRITABLE_HH
#define TYPES_WRITABLE_HH

#include "string.hh"
#include <ostream>
#include <string>

class writable
{
public:
	virtual writable& write_char (char c) = 0;
	virtual writable& write_string (const string_t&) = 0;

	virtual ~writable () = default;
};


/// Writing to std::ostream

class writable_ostream
	: public writable
{
	std::ostream& os;

public:
	writable_ostream (std::ostream& os)
		: os (os)
	{
	}

	writable_ostream () = delete;
	writable_ostream (const writable_ostream&) = default;
	writable_ostream& operator = (const writable_ostream&) = delete;

	virtual writable& write_char (char c) override {
		os.put (c);
		return *this;
	}

	virtual writable& write_string (const string_t& s) override {
		os << s;
		return *this;
	}

	virtual ~writable_ostream () = default;
};


/// Writing to std::string

class writable_string
	: public writable
{
	std::string& str;

public:
	writable_string (std::string& str)
		: str (str)
	{
	}

	writable_string () = delete;
	writable_string (const writable_string&) = default;
	writable_string& operator = (const writable_string&) = delete;

	virtual writable& write_char (char c) override {
		str.push_back (c);
		return *this;
	}

	virtual writable& write_string (const string_t& s) override {
		str.append (s.begin (), s.end ());
		return *this;
	}

	virtual ~writable_string () = default;
};

#endif
