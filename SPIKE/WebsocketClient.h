#pragma once
#include "Websocket.h"
#include "HttpClient.h"
#include "NetworkClient.h"

class WebsocketClient
{
private:
	NetworkClient& netClient;
public:
	WebsocketClient() = default;
	WebsocketClient(NetworkClient& client) : netClient(client) {}
public:
	Websocket Connect(const std::string& path)
	{
		auto chan = netClient.GetChannel();
		auto req = Request{};
		req.Method = "GET";
		req.Path = path;
		req.Headers.Set("Upgrade", "websocket");
		req.Headers.Set("Connection", "Upgrade");
		req.Headers.Set("Sec-WebSocket-Version", "13");
		req.Headers.Set("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
		auto res = HttpClient::MakeRequest(req, chan);

		if (auto upg = res.HEADERS.Get("Upgrade");(res.RESPONSE_CODE != Response::RESPONSE_TYPE::SWITCHING_PROTOCOLS) || !upg.has_value() || *upg != "websocket")
		{
			throw std::runtime_error("Websocket upgrade failed with status code: " + std::to_string(static_cast<int>(res.RESPONSE_CODE)));
		}

		return Websocket(std::move(chan));
	}
};