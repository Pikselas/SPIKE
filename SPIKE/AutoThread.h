#include <thread>
#include <memory>
#include <chrono>
#include <functional>
#include <condition_variable>

class AutoThread
{
    private:
        struct shared_thread_data
        {
            bool signal_run;
            std::move_only_function<void()> task;
            std::condition_variable& notifier;
            std::function<void()> expire_callback;
            
            shared_thread_data(std::condition_variable& notifier) : signal_run(false) , notifier(notifier) {}
        };
    private:
        std::shared_ptr<shared_thread_data> shared_data;
    public:
        AutoThread(std::condition_variable& notifier , std::chrono::milliseconds timeout);
    public:
        void setTask(std::move_only_function<void()> task);
        void setExpireCallback(std::function<void()> callback);

};

AutoThread::AutoThread(std::condition_variable& notifier , std::chrono::milliseconds timeout) : shared_data(std::make_shared<shared_thread_data>(notifier))
{
    std::thread([=]()
    {
        std::mutex mtx;
        std::unique_lock<std::mutex> m_lock(mtx);
        while(shared_data->notifier.wait_for(m_lock , timeout , [&](){  return shared_data->signal_run; }))
        {
            shared_data->signal_run = false;
            shared_data->task();
            
            // reset task so that the function gets destroyed if necessary
            // this is important if the task is a lambda function/functor and needs to free resources
            shared_data->task = nullptr;
        }
        shared_data->expire_callback();
    }).detach();
}

void AutoThread::setTask(std::move_only_function<void()> task)
{
	shared_data->task = std::move(task);
	shared_data->signal_run = true;
}

void AutoThread::setExpireCallback(std::function<void()> callback)
{
    shared_data->expire_callback = callback;
}