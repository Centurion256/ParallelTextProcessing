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

template <class T>
TSQueue<T>::TSQueue(){};

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
    cv_m.wait(lg, [this]() { return queue_m.size() != 0; });
    T value = queue_m.front();
    queue_m.pop_front();
    return value;
};

template <class T>
void TSQueue<T>::push(T value)
{
    std::unique_lock<std::mutex> lg{m_m};
    queue_m.push_back(value);
    cv_m.notify_one();
};

template class TSQueue<std::string>;
template class TSQueue<int>;