#pragma once
#include<iostream>
#include "HttpRoute.h"
#include "HeadParser.h"
#include "NetworkChannel.h"

class HttpHandler
{
private:
	using PATH_FUNCTION_T = std::function<void(Request&, Response&)>;
private:
	constexpr static float VERSION = 1.1;
private:
	std::shared_ptr<HttpRoute> HOME_ROUTE = std::make_shared<HttpRoute>(nullptr);
public:
	std::shared_ptr<HttpRoute> OnPath(const std::string& path, PATH_FUNCTION_T func)
	{
		return HOME_ROUTE->addRelativeChildRoutes(path, func);
	}
	std::shared_ptr<HttpRoute> GetHomeRoute()
	{
		return HOME_ROUTE;
	}
public:
	void operator()(NetworkChannel& CHANNEL)
	{
		try
		{
			std::string_view END_OF_SECTION = "\r\n\r\n";
			std::vector<char> buff(100);
			buff.reserve(1000);
			auto raw_point = buff.data();
			auto size_to_skip = 0u;
			PATH_FUNCTION_T path_func = nullptr;
			std::unique_ptr<Request> request;
			std::unique_ptr<Response> response;
			std::span<char> body_span;
			while (auto recv_stat = CHANNEL.Receive(raw_point, 100))
			{

				auto search_pos_start = buff.begin() + std::clamp((long)size_to_skip - (long)END_OF_SECTION.size(), (long)0, (long)buff.size());
				auto search_pos_end = buff.begin() + size_to_skip + *recv_stat;

				if (auto fnd_pos = std::search(search_pos_start, search_pos_end, END_OF_SECTION.begin(), END_OF_SECTION.end()); fnd_pos != search_pos_end)
				{
					HeadParser head_parser(buff.begin(), fnd_pos);

					body_span = std::span<char>(fnd_pos + END_OF_SECTION.size(), search_pos_end);

					auto route = HOME_ROUTE->getRelativeChildRoute(head_parser.getPath());
					if (route.first)
					{
						path_func = route.first->path_function;
					}
					unsigned int size = 0;
					if (auto sz = head_parser.getHeaders().Get("Content-Length"))
					{
						size = std::stoi(*sz);
					}
					request = std::make_unique<Request>(head_parser.getPath(), head_parser.getRequestMethod(), head_parser.getHeaders(), size, route.second);
					response = std::make_unique<Response>();
					response->Body = std::make_unique<OutStream>();
					break;
				}
				else
				{
					buff.resize(buff.size() + 100);
					size_to_skip += *recv_stat;
					raw_point = buff.data() + size_to_skip;
				}
			}
			request->reader = [&, size_left = request->BODY_SIZE](std::span<char> Inpbuff) mutable ->std::optional<unsigned int>
				{
					if (size_left > 0)
					{
						if (body_span.size() > 0)
						{
							auto copy_size = min(Inpbuff.size(), body_span.size());
							std::copy_n(body_span.begin(), copy_size, Inpbuff.begin());
							body_span = body_span.subspan(copy_size);
							size_left -= copy_size;
							return static_cast<unsigned int>(copy_size);
						}
						if (auto recv_stat = CHANNEL.Receive(Inpbuff.data(), Inpbuff.size()))
						{
							size_left -= *recv_stat;
							return recv_stat;
						}
					}
					return {};
				};
			if (path_func)
			{
				try
				{
					path_func(*request, *response);
				}
				catch (const HttpException& e)
				{
					response->HEADERS.Reset();
					response->RESPONSE_CODE = Response::RESPONSE_TYPE::INTERNAL_ERROR;
					response->HEADERS.Set("Content-Length", std::to_string(e.Length()));
					response->Body = std::make_unique<OutStringStream>(e.what());
				}
			}
			else
			{
				response->RESPONSE_CODE = Response::RESPONSE_TYPE::NOT_FOUND;
				response->SendString("Not Found");
			}
			//empty scope
			{
				// completely receive the request
				std::vector<char> buff(1000);
				while (request->ReadBody(buff));

				//send the response header
				std::stringstream stream;
				stream << "HTTP/" << VERSION << ' ' << static_cast<unsigned int>(response->RESPONSE_CODE) << ' ' << Response::RESPONSE_CODES.at(static_cast<unsigned int>(response->RESPONSE_CODE)) << "\r\n";
				stream << response->HEADERS.getRaw() << "\r\n";
				auto str = stream.str();
				CHANNEL.Send(str.c_str(), str.size());
			}
			std::vector<char> buffer(50000);
			while (response->Body->State() != OutStream::STATE::EMPTY)
			{
				auto count = response->Body->Read(buffer);
				CHANNEL.Send(buffer.data(), count);
			}
		}
		catch (const NetworkException& e)
		{
			std::cout << e.what();
		}
	}
};