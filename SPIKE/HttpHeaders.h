#pragma once
#include<string>
#include<optional>
#include<sstream>
#include<unordered_map>
class HttpHeaders
{
private:
	std::unordered_map<std::string, std::string> hashmap;
public:
	void Set(const std::string& key, const std::string& value)
	{
		hashmap[key] = value;
	}
	std::optional<std::string> Get(const std::string& key) const
	{
		if (auto key_pos = hashmap.find(key); key_pos != hashmap.end())
		{
			return key_pos->second;
		}
		return std::nullopt;
	}
	void Reset()
	{
		hashmap.clear();
	}
	auto& getInlineMap()
	{
		return hashmap;
	}
	const auto& getInlineMap() const
	{
		return hashmap;
	}
	std::string getRaw() const
	{
		std::stringstream stream;
		for (const auto& key_val : hashmap)
		{
			stream << key_val.first << ':' << key_val.second << "\r\n";
		}
		return stream.str();
	}
};