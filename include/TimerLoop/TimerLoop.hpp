//定时器
#pragma once
#include "Task/Task.hpp"
#include <coroutine>
#include <chrono>
#include <deque>
#include <thread>
#include <queue>

namespace zyco{
    struct Loop{
        struct Timer{
            std::chrono::system_clock::time_point Expiretime;
            std::coroutine_handle<> mCoroutine;

            bool operator < (const Timer& other)const noexcept{
                return Expiretime > other.Expiretime;
            }

            ~Timer(){}
        };

        std::deque<std::coroutine_handle<>> mCoroQueue;
        std::priority_queue<Timer> mTimerHeap;

        void addTimer(std::chrono::system_clock::time_point Expiretime,std::coroutine_handle<> mCoroutine){
            mTimerHeap.push(Timer(Expiretime, mCoroutine));
        }

        void addTask(std::coroutine_handle<> mCoroutine){
            mCoroQueue.push_back(mCoroutine);
        }

        void runAll(){
            while(!mCoroQueue.empty() || !mTimerHeap.empty()){
                while(!mCoroQueue.empty()){
                    auto t = std::move(mCoroQueue.front());
                    mCoroQueue.pop_front();
                    t.resume();
                }

                if(!mTimerHeap.empty()){
                    auto &t = mTimerHeap.top();
                    
                    if(t.Expiretime <= std::chrono::system_clock::now()){
                        mCoroQueue.push_back(t.mCoroutine);
                        mTimerHeap.pop();
                    } else {
                        std::this_thread::sleep_until(t.Expiretime);
                    }

                }
            }
        }
    };

    struct SleepAwaiter{
        auto await_ready() const noexcept {
            return false;
        }

        void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
            mLoop.addTimer(Expiretime,coroutine);
        }

        void await_resume() const noexcept {

        }

        std::chrono::system_clock::time_point Expiretime;
        Loop& mLoop;
    };

    Task<> sleep_for(Loop& loop,std::chrono::system_clock::duration duration){
        std::chrono::system_clock::time_point Expiretime = std::chrono::system_clock::now() + duration;
        co_await SleepAwaiter(Expiretime,loop);
    }

    Task<> sleep_until(Loop& loop,std::chrono::system_clock::time_point Expiretime){
        co_await SleepAwaiter(Expiretime,loop);
    }
}