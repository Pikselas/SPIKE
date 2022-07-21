#pragma once
#include<WinSock2.h>
#include<comdef.h>
#include"Exception.h"

class NetworkException : public Exception
{
public:
	NetworkException(const int err_code, CALLPOINT calling_point = CALLPOINT::current())
		: 
	Exception(_com_error{err_code}.ErrorMessage())
	{}
};

#define NETWORK_ERROR_IF_FAILED(func_call) if(const int res = func_call; res == SOCKET_ERROR) throw NetworkException(WSAGetLastError())