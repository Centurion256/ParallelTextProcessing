#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>
#include "queue.hpp"

// template <class T>
// class TSQueue
// {
// private:
//     std::deque<T &&> queue_m;
//     std::condition_variable cv_m;
//     mutable std::mutex m_m;

// public:
//     TSQueue() {}

//     size_t size()
//     {
//         std::lock_guard<std::mutex> lg{m_m};
//         return queue_m.size();
//     }

//     T pop()
//     {
//         std::unique_lock<std::mutex> lg{m_m};
//         cv_m.wait(lg, [this]() { return queue_m.size() == 0; });
//         T value = queue_m.front();
//         queue_m.pop_front();
//         return value;
//     }

//     void push(T&& value)
//     {
//         std::unique_lock<std::mutex> lg{m_m};
//         queue_m.push_back(value);
//         cv_m.notify_one();
// }
//---------------------------------Attempt 1 BEGIN-------------------------
// template <class T>
// TSQueue<T>::TSQueue(){};

// template <class T>
// size_t TSQueue<T>::size()
// {
//     std::lock_guard<std::mutex> lg{m_m};
//     return queue_m.size();
// };

// template <class T>
// T TSQueue<T>::pop()
// {
//     std::unique_lock<std::mutex> lg{m_m};
//     cv_m.wait(lg, [this]() { return queue_m.size() != 0; });
//     T value = queue_m.front();
//     queue_m.pop_front();
//     return value;
// };


// template <class T>
// void TSQueue<T>::push(T& value)
// {
//     std::unique_lock<std::mutex> lg{m_m};
//     queue_m.push_back(std::move(value));
//     cv_m.notify_one();
// };

// template class TSQueue<std::string>;
// template class TSQueue<int>;
//--------------------------------ATTEMPT 1 END------------------
// template <class T>
// TSQueue<T>::TSQueue() = default;

template <class T>
TSQueue<T>::TSQueue(std::mutex& m) : dispatch_mutex_m(m){};

template <class T>
size_t TSQueue<T>::size()
{
    std::lock_guard<std::mutex> lg{m_m};
    return queue_m.size();
};

template <class T>
T TSQueue<T>::pop()
{
    std::unique_lock<std::mutex> lg{m_m};
    push_pop_balance_m -= 1;
    if (push_pop_balance_m <= LOWER_BOUND)
    {
        push_pop_balance_m = 0;
        std::unique_lock<std::mutex> ntf{dispatch_mutex_m};
        dispatch_notifier_m.notify_one();
    }
    cv_m.wait(lg, [this]() { return queue_m.size() >= 1; });
    T value = queue_m.front();
    queue_m.pop_front();
    return value;
};
    
template <class T>
std::pair<T, T> TSQueue<T>::pop_two()
{
    std::unique_lock<std::mutex> lg{m_m};
    push_pop_balance_m -= 2;
    if (push_pop_balance_m <= LOWER_BOUND)
    {
        push_pop_balance_m = 0;
        std::unique_lock<std::mutex> ntf{dispatch_mutex_m};
        dispatch_notifier_m.notify_one();
    }
    cv_m.wait(lg, [this]() { return queue_m.size() >= 2; });
    T value1 = queue_m.front();
    queue_m.pop_front();
    T value2 = queue_m.front();
    queue_m.pop_front();
    std::pair<T, T> res{value1, value2};
    return res;
};

template <class T>
void TSQueue<T>::push(T value)
{   
    std::unique_lock<std::mutex> lg{m_m};
    push_pop_balance_m += 1;
    if (push_pop_balance_m >= LOWER_BOUND)
    {
        push_pop_balance_m = 0;
        std::unique_lock<std::mutex> ntf{dispatch_mutex_m};
        dispatch_notifier_m.notify_one();
    }
    queue_m.push_back(value);
    cv_m.notify_one();
};

template class TSQueue<std::string*>;
template class TSQueue<int*>;