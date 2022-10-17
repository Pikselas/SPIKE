#pragma once
#include<span>
class OutStream
{
public:
	enum class STATE
	{
		EMPTY,
		GOOD
	};
protected:
	STATE state = STATE::EMPTY;
public:
	virtual unsigned int Read(std::span<char> buffer) = 0;
	STATE State()
	{
		return state;
	}
};