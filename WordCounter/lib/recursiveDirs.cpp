#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "recursiveDirs.hpp"
#include "archive.h"
#include "archive_entry.h"

std::string readFiles(std::string path)
{
    //std::string myPath = "../test";
    std::cout << "Hello world!" << std::endl;
    using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
    std::string buffer{""};
    for (const auto &dirEntry : recursive_directory_iterator(path))
    {
        if ((dirEntry.path().extension() == ".zip"))
        {
            int res = 0;
            ssize_t size = 0;

            struct archive *a = archive_read_new();
            struct archive_entry *entry;
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);
            // char buf_c[101];
            // buf_c[0] = '\0';
            // buf_c[100] = '\0';
            // void *buf = (void *)buf_c;

            std::ifstream raw_file{dirEntry.path().string(), std::ios::binary};
            if(!raw_file){
                std::cout<<"Not opened"<<"\n";
            }
            std::ostringstream buffer_ss;
            buffer_ss << raw_file.rdbuf();
            std::string file_str = buffer_ss.str();

            if(archive_read_open_memory(a, file_str.c_str(), file_str.size())!=ARCHIVE_OK){};
            std::cout<<file_str.size()<<std::endl;
            while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
            {
                size = archive_entry_size(entry);
                std::string decoded_buffer(size, char{});
                if(archive_read_data(a, &decoded_buffer[0], decoded_buffer.size())!=ARCHIVE_OK){};
                std::cout<<decoded_buffer;
                buffer+=decoded_buffer;
                archive_entry_clear(entry);
            }
            archive_entry_clear(entry);
            archive_read_free(a);
                
            
        
                                
            
            //---------------attempt 1 BEGIN--------------------
            // int r;
            // size_t size;
            // void *buf[17];
            // size_t size;
            // void *buf[17];
            // size_t size;
            // void *buf[17];
            // struct archive_entry *entry;
            // struct archive *a = archive_read_new();
            // archive_read_support_compression_all(a);
            // archive_read_support_format_raw(a);
            // std::cout << "The name is " << dirEntry.path() << std::endl;
            // // char *content = (char *)dirEntry.path().filename().c_str();
            // // printf(content);

            // std::cout << dirEntry.path().string().c_str() << std::endl;

            // // std::string s = dirEntry.path().string();
            // // char char_array[s.length() + 1];
            // // strcpy(char_array, s.c_str());

            // std::cout << "File size is equal to :" << (size_t)dirEntry.file_size() << std::endl;
            // r = archive_read_open_filename(a, dirEntry.path().string().c_str(), (size_t)dirEntry.file_size());
            // if (r != ARCHIVE_OK)
            // {
            //     std::cout << buffer << "Here\n";
            //     return buffer;
            // }
            // r = archive_read_next_header(a, &entry);
            // if (r != ARCHIVE_OK)
            // {
            //     std::cout << buffer << "There\n";
            //     return buffer;
            // }
            // for (;;)
            // {
            //     std::cout<<"For cycle...";
            //     size = archive_read_data(a, buf, 16);
            //     std::cout<<"Read data\n"<<"\n";

            //     if (size < 0){
            //         std::cout<<"Size<0";
            //         return buffer;
            //     }
            //     if (size == 0){
            //         std::cout<<"Size==0";
            //         break;
            //     }
            //     write(1, buf, size);
            //     buffer += std::string{(char *)buf};
            // }
            // std::cout<<"Buffer"<<buffer;
            //---------------attempt 1 END--------------------

        }
        else
        {
            std::cout << dirEntry << std::endl;
            // std::fstream raw_file(dirEntry.path(), std::ios::binary);
            // // auto buffer = static_cast<std::ostringstream &>(
            // //                   std::ostringstream{} << raw_file.rdbuf());
            // // std::cout << typeid(buffer).name() << std::endl;
            // std::cout << "Printing buffer..." <<std::endl;
            // std::cout << sizeof(raw_file.rdbuf()) << std::endl;

            std::ifstream raw_file(dirEntry.path(), std::ios::binary);
            std::ostringstream buffer_ss;
            buffer_ss << raw_file.rdbuf();
            buffer += buffer_ss.str();
            std::cout << buffer << std::endl;
        }
    }
    return buffer;
}