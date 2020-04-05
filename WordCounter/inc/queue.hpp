#ifndef TYPE_SAFE_QUEUE
#define TYPE_SAFE_QUEUE

#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>
//-------- Attempt 1 BEGIN------------------
// template <class T>
// class TSQueue
// {
// private:
//     std::deque<T> queue_m;
//     std::condition_variable cv_m;
//     mutable std::mutex m_m;

// public:
//     TSQueue();
//     size_t size();
//     T pop();
//     void push(T& value);
//     T& trigger_front();
// };
//-------- Attempt 1 END------------------
template <class T>
class TSQueue
{
private:
    std::deque<T> queue_m;
    std::condition_variable cv_m;
    std::condition_variable dispatch_notifier_m;
    mutable std::mutex m_m;
    std::mutex& dispatch_mutex_m;

    

public:
    // TSQueue();
    TSQueue(std::mutex& m);
    size_t size();
    T pop();
    std::pair<T, T> pop_two();
    void push(T value);
    int push_pop_balance_m;
    // mutable std::mutex dispatch_mutex_m;
    static const int8_t LOWER_BOUND = -10;
    static const int8_t UPPER_BOUND = 10;
};
#endif