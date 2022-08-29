#include "NetworkChannel.h"

NetworkChannel::NetworkChannel(SOCKET s) : CONNECTION_SOCKET(s)
{}

NetworkChannel::NetworkChannel(NetworkChannel&& channel) noexcept
{
	*this = std::move(channel);
}

NetworkChannel& NetworkChannel::operator=(NetworkChannel&& channel) noexcept
{
	CONNECTION_SOCKET = channel.CONNECTION_SOCKET;
	channel.CONNECTION_SOCKET = INVALID_SOCKET;
	return *this;
}

NetworkChannel::~NetworkChannel()
{
	Disconnect();
}

void NetworkChannel::Send(const char* source, const unsigned int length)
{
	NETWORK_ERROR_IF_FAILED(send(CONNECTION_SOCKET, source, length , 0));
}

std::optional<unsigned int> NetworkChannel::Receive(char* dest, const unsigned int amount)
{
	auto Res = recv(CONNECTION_SOCKET, dest, amount, 0);
	NETWORK_ERROR_IF_FAILED(Res);
	if (Res > 0)
	{
		return Res;
	}
	return {};
}

void NetworkChannel::Disconnect()
{
	shutdown(CONNECTION_SOCKET, SD_BOTH);
	closesocket(CONNECTION_SOCKET);
	CONNECTION_SOCKET = INVALID_SOCKET;
}