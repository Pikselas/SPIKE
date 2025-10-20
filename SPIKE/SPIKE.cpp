#include <iostream>
#include "WebServer.h"
#include "HttpHandler.h"
#include "WebsocketHandler.h"

#include "HttpClient.h"

int main()
{
    try
    {
      HttpHandler handler;
      handler.OnPath("/", [](Request& req, Response& res) 
          {
              auto h = req.Headers.getRaw();
              res.SendString(h);
          });
      
      WebsocketHandler wHandler;

      wHandler.onUpgrade = [&](Websocket socket) -> Crotine::Task<void>
          {
              std::thread([&]() 
              {
                std::string ss;
                while (true)
                {
                    std::cin >> ss;
                    socket.Send(ss);
                }
              }).detach();
              while (true)
              {
                  auto frame = co_await await_Receive_Websocket_Frame{ socket };
                  if (frame.getOpcode() == WebsocketFrame::OPCODE::TEXT)
                  {
                      std::cout << std::string_view(frame.getPayload());
                  }
                  else if (frame.getOpcode() == WebsocketFrame::OPCODE::PING)
                  {
                      socket.Send(WebsocketFrame::ConstructPongFrame(frame.getPayload()));
                  }
                  else if (frame.getOpcode() == WebsocketFrame::OPCODE::CLOSE)
                  {

                  }
              }
          };
      std::thread{ []() 
          {
              std::cout
				  << "Client started\n";
              std::string msg;
			  std::cin >> msg;

			  NetworkClient net_client{ "127.0.0.1" , "3456" };
              HttpClient http_client(net_client);
			  auto response = http_client.Get("/");

              while (response.Body->State() != OutStream::STATE::EMPTY)
              {
				  std::vector<char> buffer(1024);
				  auto count = response.Body->Read(buffer);
				  std::cout << std::string_view(buffer.data(), count);
              }

          } }.detach();
      /*WebServer{ "3456" }.Serve([](NetworkChannel chan) -> Crotine::Task<void>
          {
			  std::vector<char> buffer(1024);
			  auto amt = co_await await_Receive_Channel_Data{ chan, buffer.data(), (UINT)buffer.size() };
			  std::cout << std::string_view(buffer.data(), amt);
          });*/

      WebServer{ "3456" }.Serve(handler);
    }
    catch (const Exception& e)
    {
        std::cerr << e.what();
    }
    return 0;
}
