#pragma once
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<optional>
#include"NetworkException.h"
#pragma comment (lib, "Ws2_32.lib")

class NetworkChannel
{
private:
	static unsigned int COUNT;
protected:
	NetworkChannel();
	~NetworkChannel();
private:
	SOCKET socket;
public:
	void Send(const char * source , const unsigned int length);
	void Send(const auto Iterable);
	bool Receive(char * dest , const unsigned int amount);
	void Disconnect();
};