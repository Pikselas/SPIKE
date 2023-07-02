#pragma once
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<optional>
#include"NetworkException.h"

#pragma comment (lib, "Ws2_32.lib")

class NetworkChannel
{
	friend class NetworkServer;
private:
	NetworkChannel(SOCKET s);
public:
	NetworkChannel(const NetworkChannel&) = delete;
	NetworkChannel(NetworkChannel&& channel) noexcept;
	NetworkChannel& operator= (const NetworkChannel&) = delete;
	NetworkChannel& operator= (NetworkChannel&& channel) noexcept;
	~NetworkChannel();
private:
	SOCKET CONNECTION_SOCKET;
public:
	void Send(const char * source , const unsigned int length);
	std::optional<unsigned int> Receive(char * dest , const unsigned int amount);
	void Disconnect();
};