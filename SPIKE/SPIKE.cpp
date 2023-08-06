#include <iostream>
#include<span>
#include<ranges>
#include"HttpServer.h"

int main()
{
    try
    {
      HttpServer server("3456");
     
      server.TempPath("/<...>", [](Request& req , Response& res)
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
