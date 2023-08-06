#include <iostream>
#include<span>
#include<ranges>
#include"HttpServer.h"

int main()
{
    try
    {
      HttpServer server("3456");
     
        server.OnPath("/<...>", [](Request& req , Response& res)
        {
            res.SendString("Hello World");
        }) -> 
        addRelativeChildRoutes("/Okiedokie", [](Request& req , Response& res)
		{
			res.SendString("Hello Okiedokie , You sent " + req.PATH_DATA.front());
        }) ->
        addRelativeChildRoutes("/<...>/Pokie", [](Request& req, Response& res)
        {
            res.SendString("Hello Pokie , You sent " + req.PATH_DATA.back());
        });

      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
