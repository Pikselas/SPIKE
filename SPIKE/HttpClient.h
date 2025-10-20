#pragma once
#include "Request.h"
#include "Response.h"
#include "HeadParser.h"
#include "HttpHandler.h"
#include "NetworkClient.h"

class HttpClient
{
private:
	NetworkClient& netClient;
public:
	HttpClient() = default;
	HttpClient(NetworkClient& client);
public:
	Response Get(const std::string& path);
	Response DoRequest(const Request& req);
};