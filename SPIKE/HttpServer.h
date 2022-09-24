#pragma once
#include<vector>
#include<thread>
#include<unordered_map>
#include"NetworkServer.h"
#include"HeadParser.h"
#include"Request.h"
#include"Response.h"
class HttpServer
{
	using PATH_FUNCTION_T = std::function<void(Request&, Response&)>;
	using PATH_FUNCTION_MAP_T = std::unordered_map<std::string , PATH_FUNCTION_T>;
public:
	constexpr static float VERSION = 1.1f;
private:
	NetworkServer SERVER;
private:
	PATH_FUNCTION_MAP_T PATH_FUNCTIONS;
private:
	class Handler
	{
	private:
		NetworkChannel CHANNEL;
	public:
		Handler(NetworkChannel&& chan) : CHANNEL(std::move(chan)){}
		void operator()(const PATH_FUNCTION_MAP_T& func_map)
		{
			std::string END_OF_SECTION = "\r\n\r\n";
			std::vector<char> buff(100);
			auto raw_point = buff.data();
			auto size_to_skip = 0u;
			PATH_FUNCTION_T path_func = nullptr;
			std::unique_ptr<Request> request;
			std::unique_ptr<Response> response;
			while (auto recv_stat = CHANNEL.Receive(raw_point, 100))
			{
				if (auto fnd_pos = std::search(buff.rbegin(), buff.rend(), END_OF_SECTION.rbegin(), END_OF_SECTION.rend()); fnd_pos != buff.rend())
				{
					HeadParser hp(buff.begin(), fnd_pos.base() - END_OF_SECTION.length());
					if (auto res = func_map.find(hp.getPath()); res != func_map.end())
					{
						path_func = res->second;
					}
					request = std::make_unique<Request>(hp.getPath(),hp.getRequestMethod(), hp.getHeaders());
					response = std::make_unique<Response>([this](const char* str, const size_t len) { CHANNEL.Send(str, len); },VERSION);
					break;
				}
				else
				{
					buff.resize(buff.size() + 100);
					size_to_skip += *recv_stat;
					raw_point = buff.data() + size_to_skip;
				}
			}
			if (path_func)
			{
				path_func(*request, *response);
			}
			else
			{
				response->RESPONSE_CODE = Response::RESPONSE_TYPE::NOT_FOUND;
				response->SendString("Not Found");
			}

		}
	};
public:
	HttpServer(const std::string& port) : SERVER(port) {}
	void OnPath(const std::string& path , PATH_FUNCTION_T func)
	{
		PATH_FUNCTIONS[path] = func;
	}
	void Serve()
	{
		while (true)
		{
			std::thread(Handler{ SERVER.GetChannel() }, std::ref(PATH_FUNCTIONS)).detach();
		}
	}
};

