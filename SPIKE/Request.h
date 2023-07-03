#pragma once
#include<span>
#include<functional>
#include"HttpHeaders.h"

class Request
{
	friend class HttpServer;
private:
	std::function<std::optional<unsigned int>(std::span<char> buff)> reader;
public:
	const std::string PATH;
	const std::string METHOD;
public:
	const unsigned int BODY_SIZE = 0;
public:
	const HttpHeaders HEADERS;
public:
	Request(const std::string& path , const std::string& method , const HttpHeaders& headers , const unsigned int body_size = 0) : PATH(path) , METHOD(method) , HEADERS(headers) , BODY_SIZE(body_size) {}
public:
	std::optional<unsigned int> ReadBody(std::span<char> buff)
	{
		return reader(buff);
	}
};