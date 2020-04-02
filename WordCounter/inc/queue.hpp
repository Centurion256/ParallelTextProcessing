#ifndef TYPE_SAFE_QUEUE
#define TYPE_SAFE_QUEUE

#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>

template <class T>
class TSQueue
{
private:
    std::deque<T> queue_m;
    std::condition_variable cv_m;
    mutable std::mutex m_m;

public:
    TSQueue();
    size_t size();
    T pop();
    void push(T value);
};

#endif