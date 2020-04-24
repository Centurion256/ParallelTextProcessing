#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <string>
//#include <functional>

#include <archive.h>
#include <archive_entry.h>

// // #include "workers.hpp"
// #include "words.hpp"
#include "configs.hpp"
#include "ThreadPool.hpp"
#include "workers.hpp"

#include <vector>
#include <map>
#include <atomic>
#include <algorithm>
// --------------------------------Time functions begin
#include <chrono>
#include <atomic>

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_ms(const D& d)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}
// -------------------------------Time functions end--------------------
#define LIMIT 10000
#define OK 0


int main(int argc, char *argv[])
{
    config conf;
    std::cout<<(argc>=2?argv[1]:"example.conf")<<std::endl;
    try
    {
        if (read_configs(conf, (argc>=2?argv[1]:"example.conf")))
        {
            throw std::runtime_error{"Error reading configs"};
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    // std::cout << "Instantiated config" << std::endl;
    

    ThreadPool<std::map<std::string, int>*> pool(conf.max_threads, conf.boundary);
    std::promise<int> res_promise;
    std::future<int> res_future = res_promise.get_future();

    auto start_time = get_current_time_fenced();

    pool.add_task([&pool, &conf, &res_promise]{worker_read(std::ref(pool), conf.data_path.string(), std::move(res_promise));});
    
    if (res_future.get() != OK)// Wait till reader finishes reading
    {
        // std::cout << "ERRROR!!!!111111" << std::endl;
        return 1;
    };
    pool.finish();

    auto finish_time = get_current_time_fenced();

    auto time = finish_time - start_time;
    std::cout<<"Working time: "<< to_ms(time) <<" ms"<<std::endl;

    std::ofstream outfile;
    std::vector<std::pair<std::string, int>> sorted;
    if (!std::filesystem::exists(conf.out_A_path.parent_path()))
    {
        std::filesystem::create_directory(conf.out_A_path.parent_path());
    }

    outfile.open(conf.out_A_path);

    for (auto it = pool.res->begin(), next_it = it; it != pool.res->end(); it = next_it)
    {
        outfile << (*it).first << ": " << (*it).second << std::endl;
        sorted.push_back((*it));
        ++next_it;
        pool.res->erase((*it).first);
    }
    outfile.close();
    outfile.open(conf.out_N_path);
    std::sort(sorted.begin(), sorted.end(), [](const std::pair<std::string, int> p1, const std::pair<std::string, int> p2)->bool{return (p1.second > p2.second) || 
                                                                                                                                        ((p1.second == p2.second) && 
                                                                                                                                        (p1.first < p2.first));
                                                                                                                                });
    for (auto it = sorted.begin(), next_it = it; it != sorted.end(); it = next_it)
    {
        outfile << (*it).first << ": " << (*it).second << std::endl;
        ++next_it;
    }
    outfile.close();

    return 0;
}