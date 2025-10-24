#pragma once
#include"NetworkChannel.h"
class NetworkServer
{
private:
	SOCKET LISTEN_SOCKET;
public:
	NetworkServer(const std::string& port);
	[[nodiscard]]
	NetworkChannel GetChannel() const;
	std::optional<NetworkChannel> GetChannelNonBlocking() const;
};