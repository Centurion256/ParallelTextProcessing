// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <filesystem>
// #include <boost/algorithm/string.hpp>

// #include "archive.h"
// #include "archive_entry.h"

#include "workers.hpp"

thread_local std::locale loc = boost::locale::generator().generate("");

const std::string PLAIN_TEXT_EXTENSIONS{".txt"};
const std::string ARCHIVE_EXTENSIONS{".zip"};

// New
void worker_read(ThreadPool<std::map<std::string, int>*>& pool, std::string path, std::promise<int>&& res){
    // std::cout<<"Started reading..."<<std::endl;
    int num = 0;
    using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
    for (const auto &dirEntry : recursive_directory_iterator(path))
    {
        // std::cout<<"Iterating"<<dirEntry.path().extension().string()<<std::endl;
        if(dirEntry.path().extension().empty())continue;// To skip dirs
        auto ext = boost::algorithm::to_lower_copy(dirEntry.path().extension().string());
        std::string* file_str;
        bool is_archive = (ARCHIVE_EXTENSIONS.find(ext)!=std::string::npos);
        bool is_text = (PLAIN_TEXT_EXTENSIONS.find(ext)!=std::string::npos);

        if(is_text){
            if((dirEntry.file_size() > 10000000)){
                // std::cout << "File " << dirEntry << " is too big" << std::endl;
                continue;
            }
        }

        if(is_archive || is_text){
            
            std::ifstream raw_file{dirEntry.path().string(), std::ios::binary};
            if (!raw_file)
            {
                // std::cout << "Not opened" << "\n";
            }
            std::ostringstream buffer_ss;
            buffer_ss << raw_file.rdbuf();
            file_str = new std::string{buffer_ss.str()};
        }

        // while(pool.num_tasks()>pool.boundary){}

        if(is_archive){
            pool.add_task(std::bind(worker_extract, std::ref(pool), file_str), true);
        }
        if(is_text){
            pool.add_task(std::bind(worker_index, std::ref(pool), file_str), true);
        }

        // std::cout << "Finished reading " <<dirEntry << std::endl; 
        num += 1;
        if(num % 100 == 0){
            std::cout<<"Read "<< num <<" files" << std::endl;
        }

    }
    res.set_value(0);
}



void worker_extract(ThreadPool<std::map<std::string, int>*>& pool, std::string* file_str){
    try{
        // std::cout<<"Started extracting..."<<std::endl;
        ssize_t size = 0;
        struct archive *a = archive_read_new();
        struct archive_entry *entry;
        archive_read_support_filter_all(a);
        archive_read_support_format_all(a);

        if (archive_read_open_memory(a, file_str->c_str(), file_str->size()) != ARCHIVE_OK)
        {
            // std::cout << "Failed reading the archive from memory" << std::endl;
        };

        while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
        {
            auto path = archive_entry_pathname(entry);
            auto ext = boost::algorithm::to_lower_copy(std::filesystem::path{path}.extension().string());
            bool is_text = (PLAIN_TEXT_EXTENSIONS.find(ext)!=std::string::npos);

            if (is_text && archive_entry_size(entry) > 10000000)
            {

                // std::cout << "File " << path << " is too big" << std::endl;
                // std::cout << "the size is:" << size << std::endl;
                continue;
            }

            if(is_text){

                std::string *d_buffer = new std::string(archive_entry_size(entry), char{});
                std::string &decoded_buffer = *d_buffer;
                // std::cout<<"Finished extracting "<<path<<std::endl;
                if (archive_read_data(a, &decoded_buffer[0], decoded_buffer.size()) != ARCHIVE_OK)
                {
                    // std::cout << "Failed reading the data from archive" << std::endl;
                    
                };

                pool.add_task(std::bind(worker_index, std::ref(pool), d_buffer));
            }
            archive_entry_clear(entry);
        }
        archive_entry_clear(entry);
        // std::cout << "Finished reading file." << std::endl;
        archive_read_free(a);
        delete file_str;
    }
    catch(const std::exception& err){
        std::cout<<err.what();
        delete file_str;
        throw err;
    }
}




void worker_index(ThreadPool<std::map<std::string, int>*>& pool, std::string* file_str){
    // std::cout<<"Started indexing..."<<std::endl;
    try{
        using namespace boost::locale::boundary;

        std::map<std::string, int>* words_count = new std::map<std::string, int>();
        // thread_local std::locale loc = pool.gen("");
        ssegment_index words_list(word, file_str->begin(), file_str->end(), loc);
        // std::cout<<"Error in index\n";
        words_list.rule(word_letters);
        std::string key;
        // std::cout << "Entered boost\n";
        for (ssegment_index::iterator it = words_list.begin(), e = words_list.end(); it != e; ++it)
        // for (auto elem:words_list)
        {

            key = boost::locale::fold_case(boost::locale::normalize((*it).str(), boost::locale::norm_type::norm_default, std::ref(loc)), std::ref(loc));
            // std::cout<<"Derefer iter is "<<key<<"\n";
            ++(*words_count)[key];
            // ++(words_count)[key];
        }
        delete file_str;
        // std::cout<<"Finished indexing"<<std::endl;
        // std::map<std::string, int>* words_count_ptr = &words_count;
        pool.merge_res([&pool](std::map<std::string, int>* first, std::map<std::string, int>* second){worker_merge(pool, first, second);}, words_count);
    }
    catch(const std::exception& err){
        std::cout<<err.what();
        delete file_str;
        throw err;
    }
    return;
}


void worker_merge(ThreadPool<std::map<std::string, int>*>& pool, std::map<std::string, int>* first, std::map<std::string, int>* second){
    // std::cout<<"Started merging..."<<std::endl;

    try{

        for(auto& elem: *second){
            (*first)[elem.first]+=elem.second;
        }
        // std::cout<<"Merged two dicts";
        delete second;
        // pool.merge_res(std::bind(worker_merge, std::ref(pool)), second);

        pool.merge_res([&pool](std::map<std::string, int>* first, std::map<std::string, int>* second){worker_merge(pool, first, second);}, std::move(first));

    }
    catch(const std::exception& err){
        std::cout<<err.what();
        delete second;
        throw err;
    }
    return;

}