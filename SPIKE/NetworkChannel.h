#pragma once
#include <list>
#include <mutex>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <optional>
#include <functional>
#include "NetworkException.h"

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
private:
	struct WsaOverlappedCustomDataField : public WSAOVERLAPPED
	{
		HANDLE _iocp;
		ULONG_PTR _key;
		DWORD _flag;
		WSABUF _buffer;
		NetworkChannel* _channel;
		std::function<void(unsigned int)> _callback;
	};
private:
	static std::mutex _receive_requests_mutex;
private:
	static std::list<WsaOverlappedCustomDataField> _receive_requests;
public:
	// Checks for completed asynchronous receive requests and calls their callbacks.
	// need to be called regularly in order to process receive events.
	static void CheckReceiveEvents();
	// Registers a new asynchronous receive request.
	static void RegisterReceiveRequest(NetworkChannel& chan , WsaOverlappedCustomDataField wsa_overlapped_data);
public:
	void Send(const char * source , const unsigned int length) const;
	void Disconnect();
public:
	std::optional<unsigned int> Receive(char* dest, const unsigned int amount) const;
	// Registers a new asynchronous receive request. The callback will be called when data is received.
	// The callback will be called from CheckReceiveEvents function.
	void SetReceiveCallback(std::function<void(unsigned int)> cb, char* dest, const unsigned int amount);
};