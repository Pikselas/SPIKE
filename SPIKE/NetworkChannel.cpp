#include "NetworkChannel.h"

std::mutex NetworkChannel::_receive_requests_mutex;
std::list<NetworkChannel::WsaOverlappedCustomDataField> NetworkChannel::_receive_requests;

NetworkChannel::NetworkChannel(SOCKET s) : CONNECTION_SOCKET(s)
{
	_iocp = CreateIoCompletionPort((HANDLE)CONNECTION_SOCKET, NULL, 0, 0);
	if (_iocp == NULL)
	{
		NETWORK_ERROR_IF_FAILED(SOCKET_ERROR);
	}
}

NetworkChannel::NetworkChannel(NetworkChannel&& channel) noexcept
{
	*this = std::move(channel);
}

NetworkChannel& NetworkChannel::operator=(NetworkChannel&& channel) noexcept
{
	CONNECTION_SOCKET = channel.CONNECTION_SOCKET;
	channel.CONNECTION_SOCKET = INVALID_SOCKET;
	_iocp = channel._iocp;
	channel._iocp = NULL;
	return *this;
}

NetworkChannel::~NetworkChannel()
{
	Disconnect();
	if (_iocp)
		CloseHandle(_iocp);
}

void NetworkChannel::Send(const char* source, const unsigned int length) const
{
	NETWORK_ERROR_IF_FAILED(send(CONNECTION_SOCKET, source, length , 0));
}

std::optional<unsigned int> NetworkChannel::Receive(char* dest, const unsigned int amount) const
{
	auto Res = recv(CONNECTION_SOCKET, dest, amount, 0);
	NETWORK_ERROR_IF_FAILED(Res);
	if (Res > 0)
	{
		return Res;
	}
	return {};
}

void NetworkChannel::CheckReceiveEvents()
{
	std::lock_guard<std::mutex> lock(_receive_requests_mutex);
	for (auto itr = _receive_requests.begin(); itr != _receive_requests.end(); )
	{
		DWORD bytes_transferred = 0;
		LPOVERLAPPED lp_overlapped = &(*itr);
		ULONG_PTR key = 0;
		if (GetQueuedCompletionStatus(itr->_iocp,&bytes_transferred, &key,&lp_overlapped,0) == TRUE)
		{
			itr->_callback(bytes_transferred);
			itr = _receive_requests.erase(itr);
			continue;
		}
		itr = std::next(itr);
	}
}

void NetworkChannel::RegisterReceiveRequest(NetworkChannel& chan , WsaOverlappedCustomDataField wsa_overlapped_data)
{
	std::lock_guard<std::mutex> lock(_receive_requests_mutex);
	auto& wsa_data = _receive_requests.emplace_back(std::move(wsa_overlapped_data));

	auto res = WSARecv(chan.CONNECTION_SOCKET , &(wsa_data._buffer), 1, nullptr, &(wsa_data._flag), &wsa_data , nullptr);
	if (res == SOCKET_ERROR)
	{
		auto err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			NETWORK_ERROR_IF_FAILED(SOCKET_ERROR);
		}
	}
}

void NetworkChannel::SetSingleReceiveCallback(std::function<void(unsigned int)> cb, char* dest, const unsigned int amount)
{
	WsaOverlappedCustomDataField wsa_overlapped_data;
	ZeroMemory(&wsa_overlapped_data, sizeof(WSAOVERLAPPED));
	wsa_overlapped_data._iocp = _iocp;
	wsa_overlapped_data._callback = cb;
	wsa_overlapped_data._flag = 0;
	wsa_overlapped_data._buffer.buf = dest;
	wsa_overlapped_data._buffer.len = amount;
	RegisterReceiveRequest(*this, std::move(wsa_overlapped_data));
}

void NetworkChannel::Disconnect()
{
	shutdown(CONNECTION_SOCKET, SD_BOTH);
	closesocket(CONNECTION_SOCKET);
	CONNECTION_SOCKET = INVALID_SOCKET;
}
