#include <iostream>
#include<thread>
#include<string>
#include<array>
#include<fstream>
#include<filesystem>
#include"NetworkServer.h"

void ConnectionHandler(NetworkChannel nc)
{
    std::array<char, 100> buff;
    std::string endofdata = "\r\n\r\n";
    while (nc.Receive(buff.data(), 100))
    {
        if (std::search(buff.rbegin(), buff.rend(), endofdata.rbegin(), endofdata.rend()) != buff.rend())
        {
            break;
        }
    }
    std::string s = "HTTP/1.1 200 OK\r\nContentn-Length:" + std::to_string(std::filesystem::file_size("E:/movies/Luck.2022.720p.Hindi,English.Esubs.MoviezVerse.in.mkv")) + endofdata;
    nc.Send(s.c_str(), s.length());
    std::ifstream fl("E:/movies/Luck.2022.720p.Hindi,English.Esubs.MoviezVerse.in.mkv", std::ios::binary);
    while (fl.good())
    {
        nc.Send(buff.data(), fl.read(buff.data(), 100).gcount());
    }
}

int main()
{
    try
    {
        NetworkServer ns("1234");
        while (true)
        {
            auto c = ns.GetChannel();
            std::thread{ ConnectionHandler , std::move(c) }.detach();
        }
     
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
