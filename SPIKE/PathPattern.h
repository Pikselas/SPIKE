#pragma once
#include<optional>
#include<string_view>
#include<algorithm>
#include<vector>
#include<regex>

class PathPattern
{
private:
    const static std::regex parser_pattern;
    constexpr static unsigned int pattern_size = 5;
private:
    std::regex path_regex;
public:
    PathPattern(const std::string_view pattern)
    {
		*this = pattern;
	}
    PathPattern operator=(const std::string_view pattern)
    {
        std::regex_iterator<std::string_view::iterator> beg(pattern.begin(), pattern.end(), parser_pattern);
        std::regex_iterator<std::string_view::iterator> end;

        std::string path_pattern;

        int lastpos = 0;

        for (auto b = beg; b != end; ++b)
        {
            std::copy_n(pattern.data() + lastpos, b->position() - lastpos, std::back_inserter(path_pattern));
            path_pattern += "([a-zA-Z0-9/.]*)";
            lastpos = b->position() + pattern_size;
        }
        std::copy_n(pattern.data() + lastpos, pattern.size() - lastpos, std::back_inserter(path_pattern));

        path_regex = path_pattern;

        return *this;
    }
    std::optional<std::vector<std::string>> operator==(const std::string_view path) const noexcept
    {
        std::cmatch match;
        if (std::regex_match(path.data(), match, path_regex))
        {
            return std::vector<std::string>{ match.begin() + 1, match.end() };
        }
        return {};
    }
};

const std::regex PathPattern::parser_pattern("<...>");

inline PathPattern operator""_pattern(const char* pattern , size_t length)
{
	return PathPattern(std::string_view(pattern , length));
}