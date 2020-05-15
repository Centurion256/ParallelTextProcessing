#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <thread>
#include <mutex>
//#include <functional>
#include <archive.h>
#include <archive_entry.h>
#include "recursiveDirs.hpp"
#include "queue.hpp"
#include <vector>
#include <map>

#define LIMIT 10000
// #include <any>
int parse_file(std::map<std::string, int>* words, std::string* file);


template <class T>
void print_stuff(TSQueue<T*> &q, std::mutex& m)
{
    // T&& front;
    while (true)
    {   
        auto front = q.pop();
        if (front == nullptr)
        {
            q.push(front);
            break;
        }
        // std::unique_ptr<T> str_ptr{front};
        {
            std::unique_lock<std::mutex> lg{m};
            std::cout << "The item in the queue is: " << *front << std::endl;
        }
        delete front;

    }
}

template <class T>
void push_stuff(TSQueue<T*> &q, std::vector<T> vls)
{
    for (auto val : vls){
        auto file_str = new std::string{val};
        q.push(file_str);
    }
    q.push(nullptr);

}

template <class T, class N>
void indexer(TSQueue<T*> &from, TSQueue<N*> &to)
{
    auto words = new std::map<std::string, int>{};
    std::string* text_ptr;
    while (true)
    {
        text_ptr = from.pop();
        if(text_ptr == nullptr){
            break;
        }
        try{
            parse_file(words, text_ptr);
            delete text_ptr;

        }
        catch(const std::exception& e){
            delete text_ptr;
            throw e;
        }

    }
    to.push(words);
}

template <class T>
void merger(TSQueue<T*> &q)
{
    while (true)
    {
        auto map_ptr_pair = q.pop_two();
        if((map_ptr_pair.first == nullptr)){
            q.push(map_ptr_pair.second);
            break;
        }
        else if((map_ptr_pair.second == nullptr)){
            q.push(map_ptr_pair.first);
            break;
        }
        for(auto& elem: *map_ptr_pair.second){
            (*map_ptr_pair.first)[elem.first]+=elem.second;
        }
        delete map_ptr_pair.second;
        q.push(map_ptr_pair.first);
    }


}

template <class T>
void thread_dispatcher(TSQueue<T*> &text_q, TSQueue<T*> &map_q, std::mutex& m, std::vector<std::thread>& threads)
{
    std::condition_variable cv;
    while (true){
        std::unique_lock<std::mutex> mtx{m};
        cv.wait(mtx);
        if (text_q.push_pop_balance_m <= text_q.LOWER_BOUND)
        {
            text_q.push_pop_balance_m = 0;
            threads.emplace_back(indexer, std::ref(text_q), std::ref(map_q));
        }
        if (text_q.push_pop_balance_m >= text_q.UPPER_BOUND)
        {
            text_q.push_pop_balance_m = 0;
            text_q.push(nullptr);
        }
        if (map_q.push_pop_balance_m <= map_q.LOWER_BOUND)
        {
            map_q.push_pop_balance_m = 0;
            threads.emplace_back(merger, std::ref(map_q));
        }
        if (map_q.push_pop_balance_m >= map_q.UPPER_BOUND)
        {
            map_q.push_pop_balance_m = 0;
            map_q.push(nullptr);
        }
    }

}


int main(int, char **)
{

    std::string path = "data";
    std::cout << "hello, world!" << std::endl;
    std::mutex dispatcher_m;
    TSQueue<std::string*> queue(std::ref(dispatcher_m));


    std::vector<std::thread> threads;
    std::mutex m;
    threads.emplace_back(print_stuff<std::string>, std::ref(queue), std::ref(m));
    threads.emplace_back(print_stuff<std::string>, std::ref(queue), std::ref(m));
    std::vector<std::string> vls = {"a", "b", "c", "d", "e", "f"};
    threads.emplace_back(push_stuff<std::string>, std::ref(queue), vls);
    for (auto& t : threads)
    {
        t.join();
    }
    // t1.join();
    // t2.jion();

    // std::cout << readFiles(path) << std::endl;
    // -----------------------attempt 3 BEGIN-------------------
    // int res = 0;
    // ssize_t size = 0;

    // struct archive *a = archive_read_new();
    // struct archive_entry *entry = archive_entry_new();
    // char buf_c[101];
    // buf_c[0] = '\0';
    // buf_c[100] = '\0';
    // void *buf = (void *)buf_c;

    // std::ifstream raw_file{"data\\Alamut.zip", std::ios::binary};
    // std::ostringstream buffer_ss;
    // buffer_ss << raw_file.rdbuf();
    // std::string file_str = buffer_ss.str();

    // archive_read_support_filter_all(a);
    // archive_read_support_format_all(a);

    // res = archive_read_open_memory(a, file_str.c_str(), file_str.size());
    // std::cout<<file_str.size()<<std::endl;

    // res = archive_read_next_header(a, &entry);
    // size = archive_entry_size(entry);

    // std::string output(size, char{});

    // res = archive_read_data(a, &output[0], output.size());
    // -----------------------------attempt 3 END------------------
    // std::cout << output << " wtf";

    // // struct archive *a = archive_read_new();
    //-------------------attempt 2 BEGIN -------------------
    // archive_read_support_compression_all(a);
    // archive_read_support_format_raw(a);
    // std::cout << "r is: " << r << std::endl;
    // r = archive_read_open_memory(a, buff, 100);
    // printf("%s",buf_c);
    //-------------------attempt 2 END -------------------

    //-------------------attempt 1 BEGIN -------------------

    // archive_read_support_compression_all(a);
    // archive_read_support_format_raw(a);
    // r = archive_read_open_filename(a, "data\\Alamut.zip", 369071 /*10240*/ /*1,006,901*/);
    // if (r != ARCHIVE_OK)
    // {
    //     std::cout << "Error open";
    // }
    // r = archive_read_next_header(a, &entry);
    // if (r != ARCHIVE_OK)
    // {
    //     std::cout << "Error read header";
    // }
    // long filesize = archive_entry_size(entry);
    // std::cout << "size is set: " << archive_entry_size(entry) << std::endl;
    // std::cout << "The size of the file is: " << filesize << std::endl;
    // std::string buff_str(filesize, char{});
    // size = archive_read_data(a, &buff_str[0], 1006900);
    // // if(size != )
    // std::cout << buff_str;

    //-------------------attempt 1 END -------------------

    // for (int i=0;i<16;i++)
    // {
    //     size = archive_read_data(a, buff, 16);
    //     if (size < 0)
    //     {
    //         /* ERROR */
    //     }
    //     if (size == 0)
    //         break;

    //     std::cout<<size<< " ";
    //     printf(buf_c-2);
    //     std::cout<<std::endl;
    //     //write(1, buff, size);
    // }

    // archive_read_free(a);

    // struct archive *a;
    // struct archive_entry *entry;
    // int r;

    // a = archive_read_new();
    // archive_read_support_filter_all(a);
    // archive_read_support_format_all(a);
    // r = archive_read_open_filename(a, "data/Alamut.zip", 0); // Note 1
    // if (r != ARCHIVE_OK)
    // {
    //     std::cout << "Error opening" << std::endl;
    //     exit(1);
    // }
    // while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    // {
    //     printf("%sHere is what we have inside\n", archive_entry_pathname(entry));
    //     archive_read_data_skip(a); // Note 2
    // }

    // r = archive_read_free(a); // Note 3
    // if (r != ARCHIVE_OK)
    // {
    //     std::cout << "Error read_free" << std::endl;
    //     exit(1);
    // }

    // std::cout << "Hello, world!\n";
    // for (int i = 0; i < LIMIT; i++)
    // {
    //     std::lock_guard<std::mutex> lg{m1};
    //     std::cout << "Hello, world!\n";
    // }
    // t1.join();
    // t2.join();

    // std::string path = "../../data/";
    // readFiles(path);
    return 0;
}