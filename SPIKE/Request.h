#pragma once
#include"HttpHeaders.h"

class Request
{
public:
	const std::string PATH;
	const std::string METHOD;
public:
	const HttpHeaders HEADERS;
};