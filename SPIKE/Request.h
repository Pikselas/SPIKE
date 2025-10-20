#pragma once
#include <span>
#include <functional>
#include "OutStream.h"
#include "HttpHeaders.h"

class Request
{
	friend class HttpHandler;
private:
	std::string Version = "HTTP/1.1";
public:
	std::string Path;
	std::string Method;
public:
	std::unique_ptr<OutStream> Body;
public:
	HttpHeaders Headers;
public:
	using PATH_DATA_T = std::vector<std::string>;
	PATH_DATA_T PATH_DATA;
public:

public:
	Request() = default;
	Request
	(
		const std::string& path , 
		const std::string& method , 
		const HttpHeaders& headers , 
		const PATH_DATA_T path_data = {}
	) : 
	Path(path) ,
	Method(method) , 
	Headers(headers) ,
	PATH_DATA(path_data) 
	{}
public:
	Request(Request&&) noexcept = default;
	Request& operator=(Request&&) noexcept = default;
public:
	std::vector<char> ToBytes() const
	{
		std::stringstream stream;
		stream << Method << ' ' << Path << ' ' << Version << "\r\n";
		stream << Headers.getRaw();
		stream << "\r\n";
		auto str = stream.str();
		// modify the body reader use the StreamClass / OutStreamClass later
		return std::vector<char>(str.begin(), str.end());
	}
};