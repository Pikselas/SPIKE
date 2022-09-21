#pragma once
#include<string>
#include<optional>
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
	auto& getInlineMap()
	{
		return hashmap;
	}
	const auto& getInlineMap() const
	{
		return hashmap;
	}
};