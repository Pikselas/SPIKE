#pragma once
#include<string>
#include<algorithm>
#include"HttpHeaders.h"
class HeadParser
{
private:
	HttpHeaders headers;
private:
	std::string PATH;
	std::string METHOD;
private:
	std::vector<std::string> split_by_delms(auto Start , auto End, const std::string& delms)
	{
		std::vector<std::string> LIST;
		auto flaggedIterStart = Start;
		auto flaggedIterEnd = std::find_first_of(Start, End, delms.begin(), delms.end());
		while (flaggedIterEnd != End)
		{
			std::string tempHolder;
			if (std::distance(flaggedIterStart, flaggedIterEnd) != 0)
			{
				std::for_each(flaggedIterStart, flaggedIterEnd, [&](const char& chr) {
					tempHolder.push_back(chr);
					});
				LIST.emplace_back(tempHolder);
			}
			flaggedIterStart = flaggedIterEnd + 1;
			flaggedIterEnd = std::find_first_of(flaggedIterStart, End, delms.begin(), delms.end());
		}
		if (std::distance(flaggedIterStart, flaggedIterEnd) != 0)
		{
			std::string tmp;
			std::for_each(flaggedIterStart, flaggedIterEnd, [&](const char& chr) {
				tmp.push_back(chr);
				});
			LIST.emplace_back(tmp);
		}
		return LIST;
	}
public:
	HeadParser(const auto Start,const auto End)
	{
		auto Vector = split_by_delms(Start, End, "\r\n");
		//first line contains request method and path
		auto FirstComponents = split_by_delms(Vector.front().begin(), Vector.front().end(), " ");
		METHOD = FirstComponents[0];
		PATH = FirstComponents[1];
		//rest of the lines are headers

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