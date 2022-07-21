#include "NetworkManager.h"

unsigned int NetworkManager::count = 0;

NetworkManager::NetworkManager()
{
	if (++count == 1)
	{
		WSADATA wdt;
		if (auto res = WSAStartup(MAKEWORD(2, 2), &wdt); res != 0)
		{
			throw NetworkException(res);
		}
	}
}

NetworkManager::~NetworkManager()
{
	if (--count == 0)
	{
		WSACleanup();
	}
}
