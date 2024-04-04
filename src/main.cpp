#include "debug.hpp"
#include "Task/Task.hpp"
#include "TimerLoop/TimerLoop.hpp"

zyco::Loop loop;

zyco::Task<int> hello1(){
    for(int i=0;i<30;i++){
        co_await sleep_for(loop,std::chrono::seconds(1));
        debug(),"hello1 1s";
    }
}

zyco::Task<> hello2(){
    for(int i=0;i<10;i++){
        co_await sleep_for(loop,std::chrono::seconds(3));
        debug(),"         hello2 3s";
    }
}

zyco::Task<int> hello_main(){
    debug(),"task begin";
    auto t1 = hello1();
    auto t2 = hello2();

    loop.addTask(t1);
    loop.addTask(t2);

    loop.runAll();

    co_return 30;
}

int main(){
    debug(),"main 创建 task";
    auto t = hello_main();
    debug(),"main 创建完 task";

    while(!t.mCoroutine.done()){
        t.mCoroutine.resume();
    }
    return 0;
}