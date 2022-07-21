#pragma once
#include<WinSock2.h>
#include"NetworkException.h"
class NetworkManager
{
	static unsigned int count;
public:
	NetworkManager();
	~NetworkManager();
};
