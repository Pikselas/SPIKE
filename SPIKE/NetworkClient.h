#pragma once
#include "NetworkChannel.h"

class NetworkClient
{
private:
	addrinfo hint, * res;
public:
	NetworkClient() = default;
	NetworkClient(const std::string& address, const std::string& port);
	~NetworkClient();
public:
	[[nodiscard]]
	NetworkChannel GetChannel() const;
};