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
      server.TempPath("/<...>/Okiedokie", [](Request& req , Response& res)
		  {
			res.SendString("Hello Okiedokie , You sent " + req.PATH_DATA.front());
		  });

      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
