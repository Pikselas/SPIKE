#pragma once
#include <functional>
#include <optional>
#include "OutStream.h"

class CustomOutStream : public OutStream
{
private:
	std::function<std::optional<unsigned int>(std::span<char> buffer)> WriteFunc;
public:
	CustomOutStream( decltype(WriteFunc) Reader )
		: 
	WriteFunc(Reader)
	{
		state = STATE::GOOD;
	}
public:
	unsigned int Read(std::span<char> buffer) override
	{
		auto result = WriteFunc(buffer);
		if (result)
		{
			return *result;
		}
		state = STATE::EMPTY;
		return 0;
	}
};