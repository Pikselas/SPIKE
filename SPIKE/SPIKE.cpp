#include <iostream>
#include<iterator>
#include"HttpServer.h"

int main()
{
    try
    {
      HttpServer server("3456");
      server.OnPath("/hell", [](auto& req, auto& res)
          {
              res.SendString("Hello World");
          });
      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
