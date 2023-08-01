#pragma once
#include<source_location>
#include<exception>
#include<sstream>
#include<filesystem>

class Exception : public std::exception
{
public:
	using CALLPOINT = std::source_location;
private:
	std::string what_buffer;
public:
	Exception(const std::string& cause, CALLPOINT calling_point = CALLPOINT::current())
	{
		std::stringstream s_stream;
#ifdef _DEBUG
		s_stream << "WHAT : " << cause << '\n'
			<< "THROWN FROM : " << std::filesystem::path(calling_point.file_name()).filename() << '\n'
			<< "FUNCTION : " << calling_point.function_name() << '\n'
			<< "ON	: " << calling_point.line();

#else
		s_stream << cause;
#endif
		what_buffer = s_stream.str();
	}
	const char* what() const override
	{
		return what_buffer.c_str();
	}
	unsigned int Length() const
	{
		return static_cast<unsigned int>(what_buffer.length());
	}
};