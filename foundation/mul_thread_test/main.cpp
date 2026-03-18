#include <thread>
#include <queue>
#include <iostream>
#include <format>
#include <condition_variable>
std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;
void push_gift(int i)  {
    {
        std::unique_lock<std::mutex> lk(mtx);
        q.push(i);
    }
    cv.notify_one();
}
void producer() {
    for(int i = 1; i < 10; ++i) {
        push_gift(i);
    }
    push_gift(0);
}
void comsumer(int id) {
    while(1) {
        
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [] {
            return !q.empty();
        });
        int i = q.front();
        if(i == 0) break;
        q.pop();
        lk.unlock();
        std::cout << std::format("线程 {} 得到了 {}\n", id, i);
    }
}
int main() {
    std::thread producer_thread(producer);
    std::thread consumer_thread1(comsumer, 1);
    std::thread consumer_thread2(comsumer, 2);
    consumer_thread1.join();
    consumer_thread2.join();
    producer_thread.join();
    return 0;
}