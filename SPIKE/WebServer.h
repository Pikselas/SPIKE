#pragma once
#include<thread>
#include<functional>
#include"NetworkServer.h"
#include"Xecutor.h"

class WebServer
{
private:
	NetworkServer SERVER;
public:
	WebServer(const std::string& port) : SERVER(port) {}
	void Serve(std::function<void(NetworkChannel&)> handler) const
	{
		using namespace std::chrono_literals;
		Xecutor xecutor{ 60000ms };

		while (true)
		{
			xecutor.execute([handler , channel = std::make_shared<NetworkChannel>(SERVER.GetChannel())]()
			{
				handler(*channel);
			});
		}
	}
};

