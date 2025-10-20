#include "HttpClient.h"

HttpClient::HttpClient(NetworkClient& client) : netClient(client)
{}

Response HttpClient::Get(const std::string & path)
{
	auto req = Request{};
	req.Method = "GET";
	req.Path = path;
	req.Headers.Set("Host", "localhost");
	return DoRequest(req);
}

Response HttpClient::DoRequest(const Request& req)
{
	Response res;
	auto raw_head = req.GetRawHead();
	auto chan = netClient.GetChannel();
	chan.Send(raw_head.c_str(), (unsigned int)raw_head.size());
	while (req.Body && req.Body->State() != OutStream::STATE::EMPTY)
	{
		std::vector<char> buffer(4096);
		auto count = req.Body->Read(buffer);
		chan.Send(buffer.data(), count);
	}
	std::vector<char> buffer(4096);
	auto amt = chan.Receive(buffer.data(), (unsigned int)buffer.size());

	if (amt)
	{
		std::string head_end = "\r\n\r\n";
		auto header_End_Pos = std::search(buffer.begin(), buffer.begin() + *amt, head_end.begin(), head_end.end());
		auto head_parser = HeadParser(buffer.begin(), header_End_Pos);
		res.HEADERS = head_parser.getHeaders();
		res.RESPONSE_CODE = static_cast<Response::RESPONSE_TYPE>(head_parser.getResponseCode());
		auto content_length_opt = res.HEADERS.Get("Content-Length");
		if (content_length_opt)
		{
			auto body_buff_start = header_End_Pos + head_end.size();
			auto body_buff_end = buffer.begin() + *amt;

			struct buffer_reader_context
			{
				std::vector<char> body_buffer;
				NetworkChannel chan;
			};

			auto reader_context = std::make_shared<buffer_reader_context>();
			reader_context->body_buffer = std::vector<char>(body_buff_start, body_buff_end);
			reader_context->chan = std::move(chan);

			auto reader = HttpHandler::GetBodyStreamReader(std::stoul(*content_length_opt), reader_context->body_buffer , reader_context->chan);

			auto reader_wrapper = [reader_context , reader](std::span<char> buff) mutable
				{
					return reader(buff);
				};
			res.Body = std::make_unique<CustomOutStream>(std::move(reader_wrapper));
		}
		else
		{
			res.Body = std::make_unique<OutStream>();
		}
	}

	return res;
}
