#include "debug.hpp"
#include "Task/Task.hpp"

zyco::Task<int> hello1(){
    debug(),"hello1";
    co_return 20;
}

zyco::Task<> hello2(){
    debug(),"hello2";
    co_return;
}

zyco::Task<int> hello_main(){
    debug(),"task begin";
    auto t1 = hello1();
    auto t2 = hello2();

    t1.mCoroutine.resume();

    co_return 30;
}

int main(){
    debug(),"main 创建 task";
    auto t = hello_main();
    debug(),"main 创建完 task";

    while(!t.mCoroutine.done()){
        t.mCoroutine.resume();
        debug(),t.mCoroutine.promise().result();
    }
    return 0;
}