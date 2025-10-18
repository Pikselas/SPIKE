#pragma once
#include <span>
#include "NetworkChannel.h"
#include "WebsocketFrame.h"

class Websocket
{
private:
	NetworkChannel CHANNEL;
public:
	Websocket() = default;
	Websocket(NetworkChannel chan) : CHANNEL(std::move(chan)) {}
public:
	void Send(const WebsocketFrame& frame)
	{
		auto bytes = frame.ToBytes();
		CHANNEL.Send(bytes.data(), bytes.size());
	}
	void Send(std::string_view data)
	{
		Send(WebsocketFrame::ConstructTextFrame(data));
	}
	WebsocketFrame Receive()
	{
		std::vector<char> buff(1000);
		auto amt = CHANNEL.Receive(buff.data(), buff.size());
		return WebsocketFrame::ConstructFrom(std::span<char>(buff.begin() , buff.end()));
	}
	// registers a callback for a single message receive
	void RegisterSingleReceiveRequest(std::function<void(WebsocketFrame)> cb , char* buffer , unsigned int amount)
	{
		CHANNEL.SetSingleReceiveCallback([=](unsigned int amt) 
		{
			cb(WebsocketFrame::ConstructFrom(std::span<char>(buffer, amt)));
		}, buffer, amount);
	}
};