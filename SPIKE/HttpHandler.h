#pragma once
#include<iostream>
#include "HttpRoute.h"
#include "HeadParser.h"
#include "NetworkChannel.h"

#include "../Crotine/Task.hpp"
#include "../Crotine/utils/Context.hpp"
#include "../Crotine/utils/Function.hpp"

class HttpHandler
{
private:
	using PATH_FUNCTION_NON_TASK_T = std::function<void(Request&, Response&)>;
	using PATH_FUNCTION_T = std::function<Crotine::Task<void>(Request&, Response&)>;
private:
	constexpr static float VERSION = 1.1;
private:
	std::shared_ptr<HttpRoute> HOME_ROUTE = std::make_shared<HttpRoute>(nullptr);
public:
	std::shared_ptr<HttpRoute> OnPath(const std::string& path, PATH_FUNCTION_T func)
	{
		return HOME_ROUTE->addRelativeChildRoutes(path, func);
	}
	std::shared_ptr<HttpRoute> OnPath(const std::string& path, PATH_FUNCTION_NON_TASK_T func)
	{
		//auto tsk_fun = std::bind_front(Crotine::CreateTask<decltype(func), Request&, Response&>, std::move(func));
		return HOME_ROUTE->addRelativeChildRoutes(path, [func](Request& req, Response& res) -> Crotine::Task<void>
		{
			return Crotine::CreateTask(func ,std::ref(req) , std::ref(res));
		});
	}
	std::shared_ptr<HttpRoute> GetHomeRoute()
	{
		return HOME_ROUTE;
	}
public:
	struct HttpParsedResult
	{
		HeadParser HEAD;
		std::vector<char> BODY;
	};
	static HttpParsedResult ParseIncomingRequest(NetworkChannel& CHANNEL)
	{
		std::string_view END_OF_SECTION = "\r\n\r\n";
		std::vector<char> buff(100);
		buff.reserve(1000);
		auto raw_point = buff.data();
		auto size_to_skip = 0u;
		HttpParsedResult result;
		std::span<char> body_span;

		while (auto recv_stat = CHANNEL.Receive(raw_point, 100))
		{
			auto search_pos_start = buff.begin() + std::clamp((long)size_to_skip - (long)END_OF_SECTION.size(), (long)0, (long)buff.size());
			auto search_pos_end = buff.begin() + size_to_skip + *recv_stat;

			if (auto fnd_pos = std::search(search_pos_start, search_pos_end, END_OF_SECTION.begin(), END_OF_SECTION.end()); fnd_pos != search_pos_end)
			{
				HeadParser head_parser(buff.begin(), fnd_pos);
				body_span = std::span<char>(fnd_pos + END_OF_SECTION.size(), search_pos_end);
				result.HEAD = std::move(head_parser);
				result.BODY = std::vector<char>(fnd_pos + END_OF_SECTION.size(), search_pos_end);
				break;
			}
			else
			{
				buff.resize(buff.size() + 100);
				size_to_skip += *recv_stat;
				raw_point = buff.data() + size_to_skip;
			}
		}
		return result;
	}
	static void SendResponse(NetworkChannel& CHANNEL , Response& response)
	{
		// send the response header
		std::stringstream stream;
		stream << "HTTP/" << VERSION << ' ' << static_cast<unsigned int>(response.RESPONSE_CODE) << ' ' << Response::RESPONSE_CODES.at(static_cast<unsigned int>(response.RESPONSE_CODE)) << "\r\n";
		stream << response.HEADERS.getRaw() << "\r\n";
		auto str = stream.str();
		CHANNEL.Send(str.c_str(), str.size());

		// send the response body
		std::vector<char> buffer(50000);
		while (response.Body->State() != OutStream::STATE::EMPTY)
		{
			auto count = response.Body->Read(buffer);
			CHANNEL.Send(buffer.data(), count);
		}
	}
	static auto GetBodyStreamReader(unsigned int size_left, std::span<char> body_span, NetworkChannel& CHANNEL)
	{
		auto reader = [body_span, &CHANNEL, size_left](std::span<char> Inpbuff) mutable ->std::optional<unsigned int>
			{
				if (size_left > 0)
				{
					if (body_span.size() > 0)
					{
						auto copy_size = (std::min)(Inpbuff.size(), body_span.size());
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
		return std::move(reader);
	}
public:
	auto handleRequest(NetworkChannel& CHANNEL) -> Crotine::Task<void>
	{
		try
		{
			PATH_FUNCTION_T path_func = nullptr;
			Request request;
			Response response;

			auto parsed_result = ParseIncomingRequest(CHANNEL);
			auto route = HOME_ROUTE->getRelativeChildRoute(parsed_result.HEAD.getPath());
			if (route.first)
			{
				path_func = route.first->path_function;
			}
			unsigned int size = 0;
			if (auto sz = parsed_result.HEAD.getHeaders().Get("Content-Length"))
			{
				size = std::stoi(*sz);
			}
			
			request = std::move(Request{ parsed_result.HEAD.getPath(), parsed_result.HEAD.getRequestMethod(), parsed_result.HEAD.getHeaders(), route.second });
			response.Body = std::make_unique<OutStream>();
			std::span<char> body_span = parsed_result.BODY;
			request.Body = std::make_unique<CustomOutStream>(GetBodyStreamReader(size , body_span , CHANNEL));
			
			if (path_func)
			{
				try
				{
					auto& cntx = co_await Crotine::get_Execution_Context{};
					co_await Crotine::RunTask(cntx, path_func, std::ref(request), std::ref(response));
					//path_func(*request, *response);
				}
				catch (const HttpException& e)
				{
					response.HEADERS.Reset();
					response.RESPONSE_CODE = Response::RESPONSE_TYPE::INTERNAL_ERROR;
					response.HEADERS.Set("Content-Length", std::to_string(e.Length()));
					response.Body = std::make_unique<OutStringStream>(e.what());
				}
			}
			else
			{
				response.RESPONSE_CODE = Response::RESPONSE_TYPE::NOT_FOUND;
				response.SendString("Not Found");
			}
			//empty scope
			{
				// completely receive the request
				std::vector<char> buff(1000);
				while (request.Body->Read(buff));
			}
			
			// send the response
			SendResponse(CHANNEL, response);
		}
		catch (const NetworkException& e)
		{
			std::cout << e.what();
		}
	}
	auto operator()(NetworkChannel CHANNEL) -> Crotine::Task<void>
	{
		auto coro = handleRequest(CHANNEL);
		coro.set_execution_ctx(co_await Crotine::get_Execution_Context{});
		coro.execute_async();
		co_await coro;
	}

	auto operator()(std::reference_wrapper<NetworkChannel> CHANNEL) -> Crotine::Task<void>
	{
		auto coro = handleRequest(CHANNEL);
		coro.set_execution_ctx(co_await Crotine::get_Execution_Context{});
		coro.execute_async();
		co_await coro;
	}
};