#pragma once
#include<sstream>
#include"OutStream.h"
class OutStringStream : public OutStream
{
	std::stringstream s;
public:
	OutStringStream(const std::string& str)
	{
		state = STATE::GOOD;
		s << str;
	}
	OutStringStream(const std::stringstream& stream)
	{
		state = STATE::GOOD;
		s << stream.str();
	}
	unsigned int Read(std::span<char> buffer) override
	{
		if (s.good())
		{
			return s.read(buffer.data(), buffer.size()).gcount();
		}
		state = STATE::EMPTY;
		return 0;
	}
};