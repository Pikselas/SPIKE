#include <iostream>
#include"HttpServer.h"

int main()
{
    try
    {
      HttpServer server("3456");
      server.OnPath("/", [](auto& req, auto& res) {
          
          res.SendString("Hello World");
          res.SendString("Another Hello");

          });
      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
