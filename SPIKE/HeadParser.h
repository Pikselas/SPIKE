#pragma once
#include <span>
#include <array>
#include <string>
#include <ranges>
#include <algorithm>
#include "HttpHeaders.h"

class HeadParser
{
private:
	HttpHeaders headers;
private:
	std::string PATH;
	std::string METHOD;
public:
	HeadParser() = default;
	HeadParser(const auto Start,const auto End)
	{
		Parse(std::span<char>(Start, End));
	}
	void Parse(const std::span<char> sp)
	{
		auto line_splitted = sp | std::ranges::views::split(std::array{ '\r' , '\n' });
		
		// first line conatins path and request method seperated by space
		auto path_and_method = line_splitted.front() | std::ranges::views::split(' ');
		METHOD = std::string(path_and_method.front().begin(), path_and_method.front().end());
		
		auto path_range = *std::next(path_and_method.begin());
		PATH = std::string( path_range.begin() , path_range.end() );

		// rest of the lines are headers
		// split_view does not works with views::drop
		for (auto header_elem = std::next(line_splitted.begin()) ; header_elem != line_splitted.end() ; header_elem++)
		{
			auto pos = std::ranges::search(*header_elem, std::array{ ':' });
			
			// trim the key and value
			auto trim_from_first_key = std::ranges::subrange((*header_elem).begin(), pos.begin()) | std::ranges::views::drop_while(isspace);
			auto trim_from_last_key = trim_from_first_key | std::views::reverse | std::views::drop_while(isspace) | std::views::reverse;
			auto header_name = std::string(trim_from_last_key.begin(), trim_from_last_key.end());

			auto trim_from_first_value = std::ranges::subrange(pos.end(), (*header_elem).end()) | std::ranges::views::drop_while(isspace);
			auto trim_from_last_value = trim_from_first_value | std::views::reverse | std::views::drop_while(isspace) | std::views::reverse;

			auto header_value = std::string(trim_from_last_value.begin(), trim_from_last_value.end());

			headers.Set(header_name, header_value);
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