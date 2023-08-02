#pragma once
#include<functional>
#include"HttpHeaders.h"
#include"ResponseLocker.h"
#include"OutStringStream.h"
#include"OutFileStream.h"
#include"OutRawStream.h"
#include"CustomOutStream.h"

class Response
{
protected:
	using ResponseWriterType = std::function<void(const char*, const unsigned int len)>;
public:
	HttpHeaders HEADERS;
public:
	enum class RESPONSE_TYPE
	{
		OK = 200,
		NOT_FOUND = 404
	};
public:
	RESPONSE_TYPE RESPONSE_CODE = RESPONSE_TYPE::OK;
public:
	const static std::unordered_map<unsigned int, std::string> RESPONSE_CODES;
protected:
	ResponseLocker locker;
public:
	std::unique_ptr<OutStream> Body;
public:
	void SendRaw(std::span<const char> raw , std::source_location cp = std::source_location::current())
	{
		locker.Lock(cp);
		HEADERS.Set("Contet-Length", std::to_string(raw.size()));
		Body = std::make_unique<OutRawStream>(raw);
	}
	void SendRaw(std::function<std::optional<unsigned int>(std::span<char>)> read_func, std::optional<size_t> max_length = std::nullopt, std::source_location cp = std::source_location::current())
	{
		locker.Lock(cp);
		Body = std::make_unique<CustomOutStream>(std::move(read_func));
		if (max_length)
			HEADERS.Set("Content-Length", std::to_string(max_length.value()));
	}
	void SendString(const std::string& str , std::source_location cp = std::source_location::current())
	{
		locker.Lock(cp);
		HEADERS.Set("Content-Length", std::to_string(str.length()));
		Body = std::make_unique<OutStringStream>(str);
	}
	void SendFile(const std::filesystem::path& path , std::source_location cp = std::source_location::current())
	{
		locker.Lock(cp);
		HEADERS.Set("Content-Length", std::to_string(std::filesystem::file_size(path)));
		Body = std::make_unique<OutFileStream>(path);
	}
};

const std::unordered_map<unsigned int, std::string> Response::RESPONSE_CODES = { {200 , "OK"} , {404 , "Not Found"}};