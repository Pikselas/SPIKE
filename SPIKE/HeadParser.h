#pragma once
#include<string>
#include<algorithm>
#include<ranges>
#include"HttpHeaders.h"
#include"ParserTools.h"
class HeadParser
{
private:
	HttpHeaders headers;
private:
	std::string PATH;
	std::string METHOD;
public:
	HeadParser(const auto Start,const auto End)
	{
		auto Vector = p_t::split_by_delms(Start, End, "\r\n");
		//first line contains request method and path
		auto FirstComponents = p_t::split_by_delms(Vector.front().begin(), Vector.front().end(), " ");
		METHOD = FirstComponents[0];
		PATH = FirstComponents[1];
		//rest of the lines are headers
		//Vector.erase(Vector.begin());
		for (auto& line : Vector | std::ranges::views::drop(0))
		{
			auto splitted = p_t::split_by_delms(line.begin(), line.end(), ":");
			headers.Set(p_t::trim(splitted.front()), p_t::trim(splitted.back()));
		}
	}
	const HttpHeaders& getHeaders()
	{
		return headers;
	}
	const std::string& getPath()
	{
		return PATH;
	}
	const std::string& getRequestMethod()
	{
		return METHOD;
	}
};