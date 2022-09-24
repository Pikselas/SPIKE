#include <iostream>
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
      server.OnPath("/ok", [](auto& req, auto& res) 
          {
              std::stringstream stream;
              stream << "<html>"
                  << "<body>"
                  << R"(<input type= "text" >)"
                  << "</body>"
                  << "</html>";
              res.SendString(stream.str());
          });
      server.Serve();
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
