#pragma once
#include<functional>
#include"OutStringStream.h"
#include"HttpHeaders.h"
#include"ResponseLocker.h"

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
	void SendString(const std::string& str , std::source_location cp = std::source_location::current())
	{
		locker.Lock(cp);
		HEADERS.Set("Content-Length", std::to_string(str.length()));
		Body = std::make_unique<OutStringStream>(str);
	}
};

const std::unordered_map<unsigned int, std::string> Response::RESPONSE_CODES = { {200 , "OK"} , {404 , "Not Found"}};