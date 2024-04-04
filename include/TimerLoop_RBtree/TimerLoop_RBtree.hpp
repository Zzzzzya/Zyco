#include "RBTREE.hpp"


//定时器
#pragma once
#include "Task/Task.hpp"
#include <coroutine>
#include <chrono>
#include <deque>
#include <thread>
#include <queue>

namespace zyco{
    struct TimerLoop{
        struct Timer{
            std::chrono::system_clock::time_point Expiretime;
            std::coroutine_handle<> mCoroutine;

            bool operator < (const Timer& other)const noexcept{
                return Expiretime < other.Expiretime;
            }

            bool operator > (const Timer& other)const noexcept{
                return Expiretime > other.Expiretime;
            }

            bool operator == (const Timer& other)const noexcept{
                return (Expiretime == other.Expiretime)&&(mCoroutine == other.mCoroutine);
            }

            ~Timer(){}
        };

        std::deque<std::coroutine_handle<>> mCoroQueue;
        rbtree<Timer> mTimertree;

        void addTimer(std::chrono::system_clock::time_point Expiretime,std::coroutine_handle<> mCoroutine){
            mTimertree.Insert(Timer(Expiretime, mCoroutine));
        }

        void addTask(std::coroutine_handle<> mCoroutine){
            mCoroQueue.push_back(mCoroutine);
        }

        void runAll(){
            while(!mCoroQueue.empty() || !mTimertree.empty()){
                while(!mCoroQueue.empty()){
                    auto t = std::move(mCoroQueue.front());
                    mCoroQueue.pop_front();
                    t.resume();
                }

                if(!mTimertree.empty()){
                    auto t = mTimertree.Min();
                    
                    if(t.Expiretime <= std::chrono::system_clock::now()){
                        mCoroQueue.push_back(t.mCoroutine);
                        mTimertree.Remove(t);
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
        TimerLoop& mLoop;
    };

    Task<> sleep_for(TimerLoop& loop,std::chrono::system_clock::duration duration){
        std::chrono::system_clock::time_point Expiretime = std::chrono::system_clock::now() + duration;
        co_await SleepAwaiter(Expiretime,loop);
    }

    Task<> sleep_until(TimerLoop& loop,std::chrono::system_clock::time_point Expiretime){
        co_await SleepAwaiter(Expiretime,loop);
    }
}