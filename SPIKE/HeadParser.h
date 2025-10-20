#pragma once
#include <span>
#include <array>
#include <string>
#include <ranges>
#include <algorithm>
#include "HttpHeaders.h"

class HeadParser
{
public:
	struct ParsedResult
	{
		std::array<std::string, 3> START_LINE;
		HttpHeaders HEADERS;
	};
private:
	ParsedResult result;
public:
	HeadParser() = default;
	HeadParser(const auto Start,const auto End)
	{
		Parse(std::span<char>(Start, End));
	}
public:
	static HttpHeaders ParseHeaders(auto ItrBeg , auto ItrEnd)
	{
		HttpHeaders headers;
		for (auto header_elem = ItrBeg; header_elem != ItrEnd; header_elem++)
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
		return headers;
	}
	void Parse(const std::span<char> sp)
	{
		auto line_splitted = sp | std::ranges::views::split(std::array{ '\r' , '\n' });
		
		// first line conatins request , path , version seperated by space for Request
		// for Response , it contains version , status code , status message
		auto first_line_splitted_by_space = line_splitted.front() | std::ranges::views::split(' ');
		
		auto first_line_part_itr = first_line_splitted_by_space.begin();
		
		for (auto& part : result.START_LINE)
		{
			part = std::string((*first_line_part_itr).begin(), (*first_line_part_itr).end());
			first_line_part_itr = std::next(first_line_part_itr);
		}

		// rest of the lines are headers
		// split_view does not works with views::drop

		auto header_lines_begin = std::next(line_splitted.begin());
		auto header_lines_end = line_splitted.end();

		result.HEADERS = ParseHeaders(header_lines_begin, header_lines_end);

		//for (auto header_elem = std::next(line_splitted.begin()) ; header_elem != line_splitted.end() ; header_elem++)
		//{
		//	auto pos = std::ranges::search(*header_elem, std::array{ ':' });
		//	
		//	// trim the key and value
		//	auto trim_from_first_key = std::ranges::subrange((*header_elem).begin(), pos.begin()) | std::ranges::views::drop_while(isspace);
		//	auto trim_from_last_key = trim_from_first_key | std::views::reverse | std::views::drop_while(isspace) | std::views::reverse;
		//	auto header_name = std::string(trim_from_last_key.begin(), trim_from_last_key.end());

		//	auto trim_from_first_value = std::ranges::subrange(pos.end(), (*header_elem).end()) | std::ranges::views::drop_while(isspace);
		//	auto trim_from_last_value = trim_from_first_value | std::views::reverse | std::views::drop_while(isspace) | std::views::reverse;

		//	auto header_value = std::string(trim_from_last_value.begin(), trim_from_last_value.end());

		//	headers.Set(header_name, header_value);
		//}
	}

	const HttpHeaders& getHeaders()
	{
		return result.HEADERS;
	}
	const std::string& getPath()
	{
		return result.START_LINE[1];
	}
	const std::string& getRequestMethod()
	{
		return result.START_LINE[0];
	}
	const unsigned int getResponseCode()
	{
		return std::atoi(result.START_LINE[1].c_str());
	}
	const std::string& getResponseText()
	{
		return result.START_LINE[2];
	}
};