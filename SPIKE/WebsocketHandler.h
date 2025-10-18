#pragma once
#include "Websocket.h"
#include "HttpHandler.h"
#include "WebsocketUtils.h"
#include "NetworkAwaiters.h"
#include "WebsocketAwaiters.h"

class WebsocketHandler
{
public:
    std::function<Crotine::Task<void>(Websocket)> onUpgrade;
public:
    Crotine::Task<void> handleRequest(NetworkChannel& chan)
    {
        try
        {
            if (onUpgrade)
                co_await Crotine::RunTask(co_await Crotine::get_Execution_Context{} , onUpgrade , Websocket{std::move(chan)});
        }
        catch (const NetworkException& e)
        {
            std::cerr << e.what();
        }
    }
public:
    Crotine::Task<void> operator()(NetworkChannel chan)
    {
        auto req = HttpHandler::ParseIncomingRequest(chan);
        auto up = req.HEAD.getHeaders().Get("Upgrade");
        Response res;
        res.Body = std::make_unique<OutStream>();
        if (up && *up == "websocket")
        {
            res.HEADERS.Set("Connection", "Upgrade");
            res.HEADERS.Set("Upgrade", "websocket");
            res.HEADERS.Set("Sec-WebSocket-Accept", ComputeWebSocketAccept(*req.HEAD.getHeaders().Get("Sec-WebSocket-Key")));
            res.RESPONSE_CODE = Response::RESPONSE_TYPE::SWITCHING_PROTOCOLS;

            HttpHandler::SendResponse(chan, res);

            co_await Crotine::TaskRunner
            { 
                co_await Crotine::get_Execution_Context{} 
            }.Run(
                [& , this]
                () -> Crotine::Task<void> 
                {
                    return handleRequest(chan);
                }
            );
        }
        else
        {

        }
    }
};