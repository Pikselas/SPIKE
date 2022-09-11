#pragma once
#include<vector>
#include"NetworkServer.h"
#include"HeadParser.h"
class HttpServer
{
private:
	constexpr static float VERSION = 1.1;
private:
	NetworkServer SERVER;
public:
	HttpServer(const std::string& port) : SERVER(port)
	{
		std::string END_OF_SECTION = "\r\n\r\n";
		while (true)
		{
			auto chan = SERVER.GetChannel();
			std::vector<char> buff(100);
			auto raw_point = buff.data();
			auto size_to_skip = 0u;
			while (auto recv_stat = chan.Receive(raw_point , 100))
			{
				if (auto fnd_pos = std::search(buff.rbegin(),buff.rend(), END_OF_SECTION.rbegin(),END_OF_SECTION.rend());fnd_pos != buff.rend())
				{
				  HeadParser(buff.begin(), fnd_pos.base() - END_OF_SECTION.length());
				  break;
				}
				else
				{
					buff.resize(buff.size() + 100);
					size_to_skip += *recv_stat;
					raw_point = buff.data() + size_to_skip;
				}
			}
			std::string s = "HTTP/1.1 200 OK\r\n\r\nHello World";
			chan.Send(s.c_str(),s.size());
		}
	}
};

