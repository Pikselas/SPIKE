#pragma once
#include"Exception.h"
class HttpException : public Exception
{
public:
	HttpException(const std::string& cause, CALLPOINT calling_point = CALLPOINT::current()) : Exception(cause , calling_point){}
};