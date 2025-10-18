#pragma once
#include <coroutine>
#include "NetworkChannel.h"
#include "Crotine/utils/Context.hpp"

class await_Receive_Channel_Data
{
private:
    NetworkChannel& CHANNEL;
    char* const buffer;
    const unsigned int length;
private:
    bool is_completed = false;
    unsigned int received_amount = 0;
public:
    await_Receive_Channel_Data(NetworkChannel& chan, char* const buffer, const unsigned int len)
        :
        CHANNEL(chan), buffer(buffer), length(len)
    {
    }
    bool await_ready() const noexcept
    {
        return is_completed;
    }
    void await_suspend(std::coroutine_handle<> handle)
    {
        CHANNEL.SetSingleReceiveCallback([handle, this](unsigned int amt)
            {
                is_completed = true;
                received_amount = amt;
                Crotine::resume_in_context(handle, Crotine::get_execution_context(handle));
            }, buffer, length);
    }
    unsigned int await_resume() const noexcept
    {
        return received_amount;
    }
};