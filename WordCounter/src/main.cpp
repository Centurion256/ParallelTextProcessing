#include <iostream>
#include <thread>
#include <mutex>
#include "recursiveDirs.hpp"
//#include <functional>
#define LIMIT 10000
// #include <any>

void print_smth(int x, std::mutex& m)
{
    for (int i = 0; i < LIMIT; i++)
    {
        std::lock_guard<std::mutex> lg{m};
        std::cout << "hello from a thread!, num =" << x << std::endl;
    }
}

int main(int, char**) {
    
    std::mutex m1;
    std::thread t1{print_smth, 2, std::ref(m1)};
    std::thread t2{print_smth, 100, std::ref(m1)};
    {
        std::lock_guard<std::mutex> lg{m1};
        std::string path = "../test";
        readFiles(path);	
    }
	std::cout << "Hello, world!\n";
    for (int i = 0; i < LIMIT; i++)
    {
        std::lock_guard<std::mutex> lg{m1};
        std::cout << "Hello, world!\n";
    }
    t1.join();
    t2.join();
    return 0;

}