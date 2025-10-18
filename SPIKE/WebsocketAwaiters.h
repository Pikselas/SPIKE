#pragma once
#include "Websocket.h"

class await_Receive_Websocket_Frame
{
private:
    Websocket& socket;
private:
    std::unique_ptr<char[]> buff;
private:
    std::optional<WebsocketFrame> frame;
public:
    await_Receive_Websocket_Frame(Websocket& socket) 
        : 
       socket(socket),
       buff(std::make_unique<char[]>(1000))
	{}
    bool await_ready() const noexcept
    {
        return frame.has_value();
    }
    void await_suspend(std::coroutine_handle<> handle)
    {
        socket.RegisterSingleReceiveRequest([this , handle](WebsocketFrame f) 
        {
            frame = std::move(f);
            Crotine::resume_in_context(handle, Crotine::get_execution_context(handle));
        }, buff.get(), 1000);
    }
    WebsocketFrame await_resume() const noexcept
    {
        return std::move(*frame);
    }
};