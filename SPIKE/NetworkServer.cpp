#include "NetworkServer.h"

NetworkServer::NetworkServer(const std::string& port)
{
    addrinfo hint, * res;
    hint = { 0 };
    hint.ai_family = AF_INET;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    auto RetriveStatus = getaddrinfo(nullptr, port.c_str(), &hint, &res);
    if (RetriveStatus != 0)
    {
        throw NetworkException(RetriveStatus);
    }
    //LISTEN_SOCKET = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	LISTEN_SOCKET = WSASocketW(res->ai_family, res->ai_socktype, res->ai_protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (LISTEN_SOCKET != INVALID_SOCKET)
    {
        auto BindResult = bind(LISTEN_SOCKET, res->ai_addr, (int)res->ai_addrlen);
        freeaddrinfo(res);
        NETWORK_ERROR_IF_FAILED(BindResult);
        NETWORK_ERROR_IF_FAILED(listen(LISTEN_SOCKET , SOMAXCONN));
    }
    else
    {
        freeaddrinfo(res);
        throw NetworkException(WSAGetLastError());
    }
}

[[nodiscard]]
NetworkChannel NetworkServer::GetChannel() const
{
    auto s = accept(LISTEN_SOCKET, nullptr, nullptr);
    if (s != INVALID_SOCKET)
    {
        return NetworkChannel(s);
    }
    else
    {
        throw NetworkException(WSAGetLastError());
    }
}

std::optional<NetworkChannel> NetworkServer::GetChannelNonBlocking() const
{
    WSAPOLLFD poll_fd
    {
		.fd = LISTEN_SOCKET,
		.events = POLLIN,
		.revents = 0
    };

    if (auto poll_res = WSAPoll(&poll_fd, 1, 0); poll_res > 0)
    {
        if(poll_fd.revents & POLLIN)
        {
			return GetChannel();
		}
    }

    return std::nullopt;
}
