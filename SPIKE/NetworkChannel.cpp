#include "NetworkChannel.h"

unsigned int NetworkChannel::COUNT = 0;

NetworkChannel::NetworkChannel()
{
	if (++COUNT == 1)
	{
		WSADATA wdt;
		if (auto res = WSAStartup(MAKEWORD(2, 2), &wdt);res != 0)
		{
			throw NetworkException(res);
		}
	}
}

NetworkChannel::~NetworkChannel()
{
	if (--COUNT == 0)
	{
		WSACleanup();
	}
}

void NetworkChannel::Send(const char* source, const unsigned int length)
{
	NETWORK_ERROR_IF_FAILED(send(socket, source, length , 0));
}

void NetworkChannel::Send(const auto Iterable)
{
	NETWORK_ERROR_IF_FAILED(send(socket, &Iterable[0], Iterable.size()));
}

bool NetworkChannel::Receive(char* dest, const unsigned int amount)
{
	auto Res = recv(socket, dest, amount, 0);
	NETWORK_ERROR_IF_FAILED(Res);
	if (Res > 0)
	{
		return true;
	}
	return false;
}

void NetworkChannel::Disconnect()
{
	shutdown(socket, SD_BOTH);
	closesocket(socket);
	socket = INVALID_SOCKET;
}