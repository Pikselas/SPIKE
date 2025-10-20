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
	return MakeRequest(req, netClient.GetChannel());
}

Response HttpClient::create_base_response(const Request& req, std::shared_ptr<buffer_reader_context> reader_context)
{
	Response res;
	auto raw_head = req.GetRawHead();
	auto& chan = reader_context->getChannel();
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

			reader_context->body_buffer = std::vector<char>(body_buff_start, body_buff_end);
			auto reader = HttpHandler::GetBodyStreamReader(std::stoul(*content_length_opt), reader_context->body_buffer, reader_context->getChannel());
			auto reader_wrapper = [reader_context, reader](std::span<char> buff) mutable
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

Response HttpClient::MakeRequest(const Request& req, NetworkChannel&& chan)
{
	auto reader_context = std::make_shared<bufer_reader_context_owning>();
	reader_context->chan = std::move(chan);
	return create_base_response(req, reader_context);
}

Response HttpClient::MakeRequest(const Request& req, NetworkChannel& chan)
{
	auto reader_context = std::make_shared<bufer_reader_context_ref>(chan);
	return create_base_response(req, reader_context);
}
