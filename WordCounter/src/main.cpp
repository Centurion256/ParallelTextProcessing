#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
//#include <functional>
#include <archive.h>
#include <archive_entry.h>
#include "recursiveDirs.hpp"
#include "queue.hpp"
#include <vector>

#define LIMIT 10000
// #include <any>

void print_smth(int x, std::mutex &m)
{
    for (int i = 0; i < LIMIT; i++)
    {
        std::lock_guard<std::mutex> lg{m};
        std::cout << "hello from a thread!, num =" << x << std::endl;
    }
}

template <class T>
void print_stuff(TSQueue<T> &q)
{
    T front;
    while (true)
    {
        front = q.pop();
        if (front == "This is the end, you know? Lady the plans we got went all wrong..")
        {
            break;
        }
        std::cout << "The item in the queue is: " << front << std::endl;
    }
}

template <class T>
void push_stuff(TSQueue<T> &q, std::vector<T> vls)
{
    for (auto &val : vls)
        q.push(val);

    q.push("This is the end, you know? Lady the plans we got went all wrong..");
}

int main(int, char **)
{
    // std::mutex m1;
    // std::thread t1{print_smth, 2, std::ref(m1)};
    // std::thread t2{print_smth, 100, std::ref(m1)};
    // std::lock_guard<std::mutex> lg{m1};

    std::string path = "data";
    std::cout << "hello, world!" << std::endl;
    // readFiles(path);
    TSQueue<std::string> queue;
    queue.push(std::string{"5"});
    queue.push(std::string{"4"});
    std::cout << queue.pop() << " " << queue.pop() << std::endl;

    std::vector<std::thread> threads;
    threads.emplace_back(print_stuff<std::string>, std::ref(queue));
    std::vector<std::string> vls = {"a", "b", "c", "d", "e", "f"};
    threads.emplace_back(push_stuff<std::string>, std::ref(queue), vls);

    // while (threads.length() != 0)
    // {
    //     for (auto thread : threads)
    //     {
    //         if thread.joinable(){
    //             thread.join;
    //         }
    //     }
    // }

    // for (int = 0; i < threads.length(); (++i) %= 2)
    // {
    //     if
    //         threads.get(i).joinable()

    // }
    for (auto &t : threads)
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