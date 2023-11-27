#include <iostream>
#include "WebServer.h"
#include "HttpHandler.h"

int main()
{
    try
    {
      HttpHandler handler;

      handler.OnPath("/", [](Request& req, Response& res) {
		  
	     res.SendString("Hello World");
	  });

      handler.OnPath("/g.mp4", [](Request& req, Response& res) {
          
         res.SendFile(R"(D:\Edgerunner.mp4)");

      });

      handler.OnPath("/greet/<...>", [](Request& req, Response& res) {
		  
		 res.SendString("Hello " + req.PATH_DATA[0] + "!!");

	  });

      handler.OnPath("/favicon.ico", [](Request& , Response& res) {
          
         res.SendFile("D:/SeqDownLogo.bmp");
      });
      WebServer server{ "3456" };
      server.Serve(handler);
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
