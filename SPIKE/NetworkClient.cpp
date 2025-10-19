#include "NetworkClient.h"

NetworkClient::NetworkClient(const std::string& address, const std::string& port)
{
	hint = { 0 };
	hint.ai_family = AF_INET;
	hint.ai_protocol = IPPROTO_TCP;
	hint.ai_socktype = SOCK_STREAM;
	auto RetriveStatus = getaddrinfo(address.c_str(), port.c_str(), &hint, &res);
	if (RetriveStatus != 0)
	{
		throw NetworkException(RetriveStatus);
	}
}

NetworkClient::~NetworkClient()
{
	freeaddrinfo(res);
}

NetworkChannel NetworkClient::GetChannel() const
{
	SOCKET connect_socket = WSASocketW(res->ai_family, res->ai_socktype, res->ai_protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (connect_socket != INVALID_SOCKET)
	{
		NETWORK_ERROR_IF_FAILED
		(
			connect(connect_socket, res->ai_addr, (int)res->ai_addrlen)
		);
	}
	else
	{
		throw NetworkException(WSAGetLastError());
	}
	return NetworkChannel(connect_socket);
}