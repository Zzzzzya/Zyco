#pragma once
#include <coroutine>

namespace zyco{
    struct RepeatableAwaiter{
        auto await_ready() const noexcept {
            return false;
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept{
            if(!coroutine.done())return coroutine;
            return std::noop_coroutine();
        }

        void await_resume()const noexcept{}
    };
}