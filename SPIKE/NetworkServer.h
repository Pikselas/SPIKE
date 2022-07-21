#pragma once
#include"NetworkChannel.h"
#include"NetworkManager.h"
class NetworkServer
{
private:
	SOCKET LISTEN_SOCKET;
	NetworkManager manager;
public:
	NetworkServer(const std::string& port);
	[[nodiscard]]
	NetworkChannel GetChannel();
};