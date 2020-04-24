#ifndef THREAD_POOL_OUR_LIB
#define THREAD_POOL_OUR_LIB

#include <thread>
#include <functional>
#include <vector>
#include <mutex>
#include <deque>
#include <condition_variable>

template <class T>
class ThreadPool{
public:
    ThreadPool(int, size_t boundary = 0);
    void add_task(std::function<void()>, bool overflow_secure=false);
    void finish();
    void merge_res(std::function<void(T, T)> merger, T new_res);
    T res;


private:
    std::vector<std::thread> workers;
    std::deque<std::function<void()>> tasks;
    std::condition_variable cv;
    std::mutex m;
    std::condition_variable overflow_cv;
    std::mutex overflow_m;
    size_t boundary;
    bool finished = false;
    void work();
};

#endif