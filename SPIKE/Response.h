#pragma once
#include<functional>
#include"HttpHeaders.h"
#include"ResponseLocker.h"

class Response
{
protected:
	const float version;
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
	ResponseWriterType write_response;
protected:
	ResponseLocker locker;
protected:
	void SendHeaders()
	{
		std::stringstream stream;
		stream << "HTTP/" << version << ' ' << static_cast<unsigned int>(RESPONSE_CODE) << ' ' << RESPONSE_CODES.at(static_cast<unsigned int>(RESPONSE_CODE)) << "\r\n";
		stream << HEADERS.getRaw() << "\r\n";
		auto str = stream.str();
		write_response(str.c_str(), str.length());
	}
public:
	Response(ResponseWriterType writer , const float version) : write_response(writer) , version(version) {}
	void SendString(const std::string& str , std::source_location cp = std::source_location::current())
	{
		locker.Lock(cp);
		HEADERS.Set("Content-Length", std::to_string(str.length()));
		SendHeaders();
		write_response(str.c_str(), static_cast<unsigned int>(str.length()));
	}
};

const std::unordered_map<unsigned int, std::string> Response::RESPONSE_CODES = { {200 , "OK"} , {404 , "Not Found"}};