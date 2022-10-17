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

          std::string ss = "Hello Hurry To World";
          res.SendRaw(ss);
          
          });
      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
