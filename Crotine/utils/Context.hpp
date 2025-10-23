#pragma once

namespace Crotine
{
    inline Executor& get_execution_context(std::coroutine_handle<> handle)
    {
        return std::coroutine_handle<PromiseBase>::from_address(
            handle.address()
        ).promise().get_execution_ctx(); 
    }

    inline void resume_in_context(std::coroutine_handle<> handle , Executor& ctx)
    {
        ctx.execute([handle]()
        {
            handle.resume();
        });   
    }

    class get_Execution_Context
    {
        private:
            std::optional<std::reference_wrapper<Executor>> _execution_context;
        public:
            bool await_ready() const noexcept
            {
                return _execution_context.has_value();
            }
            void await_suspend(std::coroutine_handle<> handle)
            {
                resume_in_context(handle ,*(_execution_context = get_execution_context(handle)));
            }
            auto await_resume() -> Executor&
            {
                if (!_execution_context.has_value())
                {
                    throw std::runtime_error("Execution pool not set");
                }
                return _execution_context->get();
            }
    };
}