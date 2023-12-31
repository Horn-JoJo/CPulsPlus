#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>

std::condition_variable_any cv;
std::mutex cv_m;
//This mutex is used for three purposes
//1) to synchronize accesses to i 同步i
//2) to synchronize accesses to std::cerr 同步标准报错
//3) for the condition variable cv  同步条件变量

int i = 0;

void waits(int idx) {
    //传入唯一锁中，可以自动加锁和解锁
    std::unique_lock<std::mutex> lk(cv_m);
    std::cerr << "Waiting... \n";
    cv.wait(lk, []{return i == 1;});//当i == 1时候不会阻塞
    std::cerr << "thread:" << idx << " ...finished waiting. i == 1\n";
}

void signals() {
    //睡眠1s
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        //lock_guard类模板会在构造函数中加锁，析构函数中解锁
        std::lock_guard<std::mutex> lk(cv_m);
        std::cerr << "Notifying...\n";
    }
    cv.notify_all();
    std::cout << "都唤换不醒" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        i = 1;
        std::cerr << "Notifying again...\n";
    }
    cv.notify_all();
    std::cout << "都給我醒" << std::endl;
}

int main() {
    //传入线程
    std::thread t1(waits, 1), t2(waits, 2), t3(waits, 3), t4(signals);
    //主线程等待子线程结束后回收资源
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
