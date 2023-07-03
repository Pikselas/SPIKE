#include <iostream>
#include<span>
#include<ranges>
#include"HttpServer.h"

int main()
{
    try
    {
      HttpServer server("3456");
      server.OnPath("/", [](Request& req, Response& res) {

          res.SendFile(R"(D:\l\retro.jpg)");

          });
      server.OnPath("/test", [](Request& req, Response& res) {
          
          if (req.METHOD == "GET")
          {
			  res.SendString("Hello World!");
		  }
          else if (req.METHOD == "POST")
          {
			  std::vector<char> buff(100);
              std::stringstream stream;
              stream << "RECEIVED::";
              while (auto read_size = req.ReadBody(buff))
              {
                stream << std::string_view(buff.data(), *read_size);
			  }
              res.SendString(stream.str());
          }
          });
      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
