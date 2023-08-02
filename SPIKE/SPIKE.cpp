#include <iostream>
#include<span>
#include<ranges>
#include"HttpServer.h"

int main()
{
    try
    {
      HttpServer server("3456");
      
      server.OnPath("/test_view", [](Request& req, Response& res) {
          
          res.SendFile("D:/CoderWallp/204.jpg");
          std::cout << *req.HEADERS.Get("User-Agent") << std::endl;
          
          });

      server.OnPath("/test_view/<...>"_pattern, [](Request& req, Response& res)
      {
          res.SendFile("D:/CoderWallp/" + (*req.PATH_DATA)[0]);
       
      });

      server.OnPath("/custom", [](auto& req, auto& res)
          {
                 res.SendRaw
                 (
                 [
                  fl = std::make_shared<std::ifstream>("D:/Virgin's First Love.mp4", std::ios::binary)
                 ]
                 (
                    std::span<char> buffer
                 ) mutable -> std::optional<unsigned int>
                {

                    if (fl->eof())
				    {
					    return std::nullopt;
				    }
                    return fl->read(std::data(buffer), std::size(buffer)).gcount();

                });

          });

      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
