#include <iostream>
#include<span>
#include<ranges>
#include"HttpServer.h"

int main()
{
    try
    {
      HttpServer server("3456");
      server.OnPath("/", [](auto& req, auto& res) {

          res.SendFile(R"(C:\Users\Aritra Maji\Downloads\Video\MIDV-216.mp4)");
          
          });
      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
