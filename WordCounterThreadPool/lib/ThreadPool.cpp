#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <deque>
#include <functional>
#include <map>
#include <iostream>
#include <ostream>
#include "ThreadPool.hpp"

template <class T>
ThreadPool<T>::ThreadPool(int nthread, size_t boundary) : boundary(boundary)
{
    for (size_t i = 0; i < nthread; i++)
    {
        workers.emplace_back(&ThreadPool::work, this);
    }
    res = nullptr;
};

template <class T>
void ThreadPool<T>::work(){
    while(true)
    {
        std::function<void()> current_task;

        {
            std::unique_lock<std::mutex> ulk{m};
            cv.wait(ulk, [&]{return (tasks.size() != 0) || finished;});
            if ((tasks.size() == 0) && finished)
            {
                return;
            }
            current_task = tasks.front();
            tasks.pop_front();
            overflow_cv.notify_one();
        }
        current_task();
    }
    return;
};

template <class T>
void ThreadPool<T>::add_task(std::function<void()> task, bool overflow_secure)
{
    {
        std::unique_lock<std::mutex> ulk{m};
        // std::cout<<this->tasks.size()<<std::endl;
        if (overflow_secure && boundary > 0)
            cv.wait(ulk, [this]{return tasks.size() < boundary;});
        tasks.push_back(task);
    }
    cv.notify_one();
};

template <class T>
void ThreadPool<T>::finish(){
    {std::unique_lock<std::mutex> ulk{m};
        finished = true;
    }
    cv.notify_all();
    for(auto &worker:workers){
        worker.join();
    }
    // std::cout << "All done" << std::endl;
};
//Apply a 'merger' function to 'res' object of the ThreadPool and object of the same type 'new_res', then reset 'res'
//If 'res' has been reset, set it to 'new_res' and return.
template <class T>
void ThreadPool<T>::merge_res(std::function<void(T, T)> merger, T new_res)
{
    T temp_res;
    // std::cout<<"Entered merge_res"<<std::endl;
    {std::lock_guard<std::mutex> lg{m};
        if(res==nullptr){
            // std::cout<<"Set res"<<std::endl;
            res = new_res;
            return;
        }
        else{
            // std::cout<<"Starting merge..."<<std::endl;
            temp_res = res;
            // std::cout<<temp_res << std::endl;
            res = nullptr;
        }
    }
    add_task(std::bind(merger, temp_res, new_res));

};

template class ThreadPool<std::map<std::string, int>*>;


