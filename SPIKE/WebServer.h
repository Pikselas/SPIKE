#pragma once
#include<thread>
#include<functional>
#include"NetworkServer.h"
#include"Xecutor.h"

#include "Crotine/Xecutor.hpp"
#include "Crotine/TaskRunner.hpp"

class WebServer
{
private:
	NetworkServer SERVER;
public:
	WebServer(const std::string& port) : SERVER(port) {}
	void Serve(std::function<Crotine::Task<void>(NetworkChannel)> handler) const
	{
		using namespace std::chrono_literals;
		Crotine::Xecutor xecutor;
		Crotine::TaskRunner runner{ xecutor };

		while (true)
		{
			NetworkChannel::CheckReceiveEvents();
			if (auto chan = SERVER.GetChannelNonBlocking(); chan.has_value())
			{
				runner.Run(handler, std::move(*chan)).detach();
			}
			/*xecutor.execute([handler, channel = SERVER.GetChannel()]() mutable
			{
				handler(channel);
			});*/
		}
	}
};

