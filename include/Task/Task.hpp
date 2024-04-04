#pragma once
#include "awaiter/PreviousAwaiter.hpp"
#include <coroutine>
#include <exception>

namespace zyco{

    template <class T>
    struct Promise{
        std::coroutine_handle<Promise<T>> get_return_object() noexcept{
            return std::coroutine_handle<Promise<T>>::from_promise(*this);
        }

        auto initial_suspend() const noexcept{
            return std::suspend_always();
        }

        auto final_suspend() const noexcept{
            return zyco::PreviousAwaiter(mPrevious);
        }

        void unhandled_exception(){
            mException = std::current_exception();
        }

        auto yield_value(T ret){
            if(mException) [[unlikely]]{
                std::rethrow_exception(mException);
            }

            mValue = ret;
            return std::suspend_always();
        }

        auto return_value(T ret){
            if(mException) [[unlikely]]{
                std::rethrow_exception(mException);
            }

            mValue = ret;
        }

        T& result(){
            if(mException) [[unlikely]]{
                std::rethrow_exception(mException);
            }

            return (T&)mValue;
        }
        
        void setPrevious(std::coroutine_handle<> previous) noexcept {
            mPrevious = previous;
        }

        std::exception_ptr mException;
        std::coroutine_handle<> mPrevious; 
        T mValue;


        Promise &operator=(Promise&&) = delete;
        ~Promise(){}
    };

    template <>
    struct Promise<void>{
        std::coroutine_handle<Promise> get_return_object() noexcept{
            return std::coroutine_handle<Promise>::from_promise(*this);
        }

        auto initial_suspend() const noexcept{
            return std::suspend_always();
        }

        auto final_suspend() const noexcept{
            return zyco::PreviousAwaiter(mPrevious);
        }

        void unhandled_exception(){
            mException = std::current_exception();
        }

        auto return_void() noexcept {}

        void result(){
            if(mException) [[unlikely]]{
                std::rethrow_exception(mException);
            }
        }
        
        void setPrevious(std::coroutine_handle<> previous) noexcept {
            mPrevious = previous;
        }

        std::exception_ptr mException;
        std::coroutine_handle<> mPrevious; 

        Promise &operator=(Promise&&) = delete;
        ~Promise(){}
    };

    template<class T = void>
    struct Task{
        using promise_type = Promise<T>;

        std::coroutine_handle<promise_type> mCoroutine;

        Task(std::coroutine_handle<promise_type> coro): mCoroutine(coro){}
        Task &operator= (Task&&) = delete;
        ~Task(){
            if(mCoroutine)
                mCoroutine.destroy();
        };

        struct Awaiter {
            bool await_ready() const noexcept {
                return false;
            }

            std::coroutine_handle<promise_type>
            await_suspend(std::coroutine_handle<> coroutine) const noexcept {
                promise_type &promise = mCoroutine.promise();
                promise.setPrevious(coroutine);
                return mCoroutine;
            }

            T await_resume() const {
                return mCoroutine.promise().result();
            }

            std::coroutine_handle<promise_type> mCoroutine;
        };

        auto operator co_await(){
            return Awaiter(mCoroutine);
        }

        operator std::coroutine_handle<>(){
            return mCoroutine;
        }
    };
}