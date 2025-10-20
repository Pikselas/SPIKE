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
private:
	struct buffer_reader_context
	{
		std::vector<char> body_buffer;
		virtual NetworkChannel& getChannel() = 0;
	};
	struct bufer_reader_context_owning : public buffer_reader_context
	{
		NetworkChannel chan;
		virtual NetworkChannel& getChannel() override { return chan; }
	};
	struct bufer_reader_context_ref : public buffer_reader_context
	{
		NetworkChannel& chan;
		bufer_reader_context_ref(NetworkChannel& channel) : chan(channel) {}
		virtual NetworkChannel& getChannel() override { return chan; }
	};
public:
	Response Get(const std::string& path);
	Response DoRequest(const Request& req);
private:
	static Response create_base_response(const Request& req, std::shared_ptr<buffer_reader_context> reader_context);
public:
	static Response MakeRequest(const Request& req, NetworkChannel&& chan);
	static Response MakeRequest(const Request& req , NetworkChannel& chan);
};