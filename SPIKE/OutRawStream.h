#pragma once
#include<algorithm>
#include"OutStream.h"
class OutRawStream : public OutStream
{
private:
	std::vector<char> buff;
public:
	OutRawStream(std::span<const char> buff) : buff(buff.begin(),buff.end()) 
	{
		state = STATE::GOOD;
	}
	unsigned int Read(std::span<char> buffer) override
	{
		auto copyCount = (std::min)(buffer.size(), buff.size());
		std::copy_n(buff.begin(), copyCount, buffer.begin());
		buff.erase(buff.begin(), buff.begin() + copyCount);
		if (buff.empty())
		{
			state = STATE::EMPTY;
		}
		return copyCount;
	}
};