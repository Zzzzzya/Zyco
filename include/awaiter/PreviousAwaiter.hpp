#pragma once
#include <coroutine>

namespace zyco{

    struct PreviousAwaiter{
        auto await_ready() const noexcept {
            return false;
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept{
            if(mPrevious)
                return mPrevious;
            return std::noop_coroutine();
        }

        void await_resume()const noexcept{}

        std::coroutine_handle<> mPrevious;
    };

}