#pragma once
#include<source_location>
#include<exception>
#include<sstream>

class Exception : public std::exception
{
private:
	std::string what_buffer;
public:
#if defined(_DEBUG)
	Exception(const std::string& cause, std::source_location s = std::source_location::current())
	{
		std::stringstream s_stream;
		s_stream << "WHAT : " << cause << '\n'
			<< "THROWN FROM : " << s.file_name() << "\n"
			<< "ON	: " << s.line();
		what_buffer = s_stream.str();
	}
#else
	Exception(const std::string& cause)
	{
		what_buffer = cause;
	}
#endif
	const char* what() const override
	{
		return what_buffer.c_str();
	}
};