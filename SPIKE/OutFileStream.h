#pragma once
#include<fstream>
#include<algorithm>
#include"OutStream.h"

class OutFileStream : public OutStream
{
private:
	std::ifstream file;
public:
	OutFileStream(const std::filesystem::path& path)
	{
		file.open(path , std::ios_base::binary);
		state = STATE::GOOD;
	}
	unsigned int Read(std::span<char> buffer) override
	{
		if (file.good())
		{
			return file.read(buffer.data() , buffer.size()).gcount();
		}
		state = STATE::EMPTY;
		return 0;
	}
	~OutFileStream()
	{
		file.close();
	}
};