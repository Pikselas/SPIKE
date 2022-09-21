#pragma once
#include<functional>
#include"HttpHeaders.h"
class Response
{
public:
	using ResponseWriterType = std::function<void(const char*, const size_t len)>;
public:
	HttpHeaders HEADERS;
public:
	ResponseWriterType response_writer;
public:
	Response(ResponseWriterType writer) : response_writer(writer) {}
	void SendString(const std::string& str) const
	{
		response_writer(str.c_str(), str.length());
	}
};