#include <iostream>
// #include "tbb/tbb.h"
#include "tbb/pipeline.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_reduce.h"
#include <filesystem>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include "archive.h"
#include "archive_entry.h"
// #include<
#include <chrono>
#include <thread>

#define N 10

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template <class D>
inline long long to_ms(const D &d)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

// void fn()
// {
//     std::cout << "Hello, TBB world!" << std::endl;
// }

// void ParallelApplyFoo(float *a, size_t n)
// {
//     tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
//                       [=](const blocked_range<size_t> &r) {
//                           for (size_t i = r.begin(); i != r.end(); ++i)
//                               Foo(a[i]);
//                       });
// }

// float innerProduct(std::vector<float> vect1, std::vector<float> vect2)
// {
//     tbb::transform_reduce(pstl::execution::par,
//     vec1.begin(), vect1.end(),vect2.begin(),
//     0.0, [](float a, float b){
//         return a + b},
//         [](float a, float b){
//         return a * b}))
// }

struct MarkedString
{
    std::string *str;
    bool is_archive;
};

thread_local std::locale loc = boost::locale::generator().generate("");

const std::string PLAIN_TEXT_EXTENSIONS{".txt"};
const std::string ARCHIVE_EXTENSIONS{".zip"};

class FilterRead
{
private:
    std::filesystem::recursive_directory_iterator *entry;
    std::filesystem::recursive_directory_iterator *end;
    // std::filesystem::recursive_directory_iterator dir_iterator;
    // std::filesystem::recursive_directory_iterator cur_entry;

public:
    FilterRead(std::filesystem::recursive_directory_iterator *direntry, std::filesystem::recursive_directory_iterator *end) : entry{direntry}, end{end}
    {
        // cur_entry = iter();
        // dir_iterator = std::filesystem::recursive_directory_iterator(path);
        // cur_entry = std::filesystem::begin(dir_iterator);
    }

    MarkedString operator()(tbb::flow_control &fc) const
    {
        // std::cout << "Call guard begin" << std::endl;
        // auto cur_entry = iter();
        // std::cout << (*entry == *end) << " " << std::endl;
        if (*end == *entry)
        {
            fc.stop();
            return MarkedString{nullptr, 0};
        }
        // std::cout << "Call guard end" << std::endl;

        // if (cur_entry == std::filesystem::end(dir_iterator))
        // if (cur_entry == std::filesystem::end(cur_entry))
        // {
        //     fc.stop();
        //     return MarkedString{nullptr, 0};
        // }

        auto dirEntry = *(*entry)++;
        // cur_entry++;

        // std::cout << dirEntry.path() << std::endl;
        if (dirEntry.path().extension().empty())
        {
            // return FilterRead::operator()(fc); // To skip dirs
            return MarkedString{nullptr, 0};
        }

        auto ext = boost::algorithm::to_lower_copy(dirEntry.path().extension().string());
        bool is_archive = (ARCHIVE_EXTENSIONS.find(ext) != std::string::npos);
        bool is_text = (PLAIN_TEXT_EXTENSIONS.find(ext) != std::string::npos);
        if (!(is_archive || is_text))
        {
            // return FilterRead::operator()(fc);
            return MarkedString{nullptr, 0};
        }
        if ((dirEntry.file_size() > 10000000))
        {
            // std::cout << "File " << dirEntry << " is too big" << std::endl;
            // return FilterRead::operator()(fc);
            return MarkedString{nullptr, 0};
        }

        std::ifstream raw_file{dirEntry.path().string(), std::ios::binary};
        std::ostringstream buffer_ss;
        buffer_ss << raw_file.rdbuf();
        std::string *file_str = new std::string{buffer_ss.str()};

        return MarkedString{file_str, is_archive};
    }
};

class ArchiveForwardFilter
{
    // private:
    //     std::vector<std::string *> files;

public:
    ArchiveForwardFilter() = default;

    std::vector<std::string *> operator()(MarkedString file_str) const
    {
        std::vector<std::string *> files{};
        if (file_str.str == nullptr)
        {
            // std::cout << "nullptr" << std::endl;
            return files;
        }
        // std::cout << *(file_str.str) << std::endl;
        if (!file_str.is_archive)
        {
            files.push_back(file_str.str);
            return files;
        }

        try
        {
            // std::cout<<"Started extracting..."<<std::endl;
            ssize_t size = 0;
            struct archive *a = archive_read_new();
            struct archive_entry *entry;
            archive_read_support_filter_all(a);
            archive_read_support_format_all(a);

            if (archive_read_open_memory(a, file_str.str->c_str(), file_str.str->size()) != ARCHIVE_OK)
            {
                // std::cout << "Failed reading the archive from memory" << std::endl;
            };

            while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
            {
                auto path = archive_entry_pathname(entry);
                auto ext = boost::algorithm::to_lower_copy(std::filesystem::path{path}.extension().string());
                bool is_text = (PLAIN_TEXT_EXTENSIONS.find(ext) != std::string::npos);

                if (is_text && archive_entry_size(entry) > 10000000)
                {

                    // std::cout << "File " << path << " is too big" << std::endl;
                    // std::cout << "the size is:" << size << std::endl;
                    continue;
                }

                if (is_text)
                {

                    std::string *d_buffer = new std::string(archive_entry_size(entry), char{});
                    std::string &decoded_buffer = *d_buffer;
                    // std::cout<<"Finished extracting "<<path<<std::endl;
                    if (archive_read_data(a, &decoded_buffer[0], decoded_buffer.size()) != ARCHIVE_OK)
                    {
                        // std::cout << "Failed reading the data from archive" << std::endl;
                    };

                    files.push_back(d_buffer);
                }
                // archive_entry_clear(entry);
            }
            // archive_entry_clear(entry);
            archive_read_free(a);
            // archive_entry_free(entry);
            // std::cout << "Finished reading file." << std::endl;
            delete file_str.str;
            // std::cout << "unarchived files" << std::endl;
        }
        catch (const std::exception &err)
        {
            std::cout << err.what();
            delete file_str.str;
            throw err;
        }
        // std::cout << "Processed an achive file." << std::endl;
        return files;
    }
};

class IndexerFilter
{
public:
    IndexerFilter() = default;
    std::map<std::string, size_t> *operator()(std::vector<std::string *> vect) const
    {
        std::map<std::string, size_t> *words_count = new std::map<std::string, size_t>();
        try
        {
            for (auto &file_str : vect)
            {
                using namespace boost::locale::boundary;
                ssegment_index words_list(word, file_str->begin(), file_str->end(), loc);
                words_list.rule(word_letters);
                std::string key;
                for (ssegment_index::iterator it = words_list.begin(), e = words_list.end(); it != e; it++)
                {
                    key = boost::locale::fold_case(boost::locale::normalize((*it).str(), boost::locale::norm_type::norm_default, std::ref(loc)), std::ref(loc));
                    ++(*words_count)[key];
                }
                key.clear();
            }
        }
        catch (const std::exception &err)
        {
            std::cout << err.what();
            throw err;
        }
        return words_count;
    }
};

class OutputFilter
{
private:
    std::vector<std::map<std::string, size_t> *> *out;
    // std::function<void(std::map<std::string, size_t> *)> exofunctor_closure;

public:
    OutputFilter(std::vector<std::map<std::string, size_t> *> *out) : out{out}
    {
    }

    void operator()(std::map<std::string, size_t> *m) const
    {
        if (!m->empty())
        {
            // exofunctor_closure(m);
            out->push_back(m);
        }
        else
        {
            std::cout << "Empty map." << std::endl;
        }
        // out_maps->push_back(m);
    }
};

class Reducer
{
    // float *my_res;
    std::vector<std::map<std::string, size_t> *> local_vector;

public:
    std::map<std::string, size_t> *res;

    Reducer(std::vector<std::map<std::string, size_t> *> v) : local_vector(v)
    {
        res = new std::map<std::string, size_t>{};
    }

    void operator()(const tbb::blocked_range<size_t> &r)
    {
        for (size_t i = r.begin(); i != r.end(); ++i)
            for (auto &elem : *(local_vector[i]))
            {
                (*res)[elem.first] += elem.second;
                // delete elem.first;
            }
    }

    Reducer(Reducer &reducer, tbb::split) : local_vector(reducer.local_vector)
    {
        res = new std::map<std::string, size_t>{};
    }

    void join(const Reducer &other)
    {
        for (auto &elem : *other.res)
        {
            (*res)[elem.first] += elem.second;
        }
    }

    ~Reducer()
    {
        delete res;
    }
};

struct config
{
    int boundary;
    std::filesystem::path data_path;
    std::filesystem::path out_A_path;
    std::filesystem::path out_N_path;
};

int read_configs(config &configs, std::string path = "example.conf")
{
    try
    {
        std::ifstream file{path};
        if (!file)
        {
            throw std::logic_error{"No such file: " + path};
            return 1;
        }
        std::string line;
        std::vector<std::string> words;
        while (std::getline(file, line))
        {
            boost::split(words, line, boost::is_any_of("=;"));
            if (words[0].find("data") != std::string::npos)
                configs.data_path = std::filesystem::path{boost::trim_copy(std::string{words[1]})};
            else if (words[0].find("boundary") != std::string::npos)
                configs.boundary = configs.boundary = std::stoi(words[1]);
            else if (words[0].find("A") != std::string::npos)
                configs.out_A_path = std::filesystem::path{boost::trim_copy(std::string{words[1]})};
            else if (words[0].find("N") != std::string::npos)
                configs.out_N_path = std::filesystem::path{boost::trim_copy(std::string{words[1]})};
        }
        file.close();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
}

// --------------------------------------------------------------------------------------------------------------------------------

int test_main(int argc, char const *argv[])
{
    config configs;
    if (argc >= 2)
    {
        if (argc == 2)
        {
            read_configs(configs, argv[1]);
        }
        else
        {
            std::cout << "Error in program arguments\n";
            return 1;
        }
    }
    else
    {
        read_configs(configs);
    }

    std::map<std::string, size_t> map1{};
    map1["abc"] = 2;
    map1["de"] = 3;

    std::map<std::string, size_t> map2{};
    map2["abc"] = 1;
    map2["deg"] = 5;

    std::vector<std::map<std::string, size_t> *> res_vector{&map1, &map2};

    for (int i = 0; i < 12; i++)
    {
        auto m = new std::map<std::string, size_t>{};
        (*m)["a"] = 2;
        res_vector.push_back(m);
    }

    // auto res = tbb::parallel_reduce(
    //     tbb::blocked_range<size_t>(0, res_vector.size()),
    //     new std::map<std::string, size_t>,

    //     [&](tbb::blocked_range<size_t> &r, std::map<std::string, size_t> *cur_res) {
    //         for (size_t i = r.begin(); i != r.end(); i++)
    //         {
    //             for (auto &elem : *(res_vector[i]))
    //             {
    //                 (*cur_res)[elem.first] += elem.second;
    //                 // delete elem.first;
    //             }
    //             // delete res_vector[i];
    //         }
    //         return cur_res;
    //     },

    //     [&](std::map<std::string, size_t> *first, std::map<std::string, size_t> *second) {
    //         std::cout<<(first==second)<<std::endl;
    //         for (auto &elem : *first)
    //         {
    //             (*second)[elem.first] += elem.second;
    //         }
    //         // delete first;
    //         return second;
    //     });
    std::cout << "Pipeline done." << std::endl;

    std::cout << res_vector.size() << std::endl;
    for (auto &i : res_vector)
    {
        std::cout << i << " ";
    }

    std::cout << "\n\n";

    // auto res = tbb::parallel_reduce(
    //     tbb::blocked_range<size_t>(0, res_vector.size()),
    //     new std::map<std::string, size_t>,

    //     [&](tbb::blocked_range<size_t> &r, std::map<std::string, size_t> *cur_res) {
    //         for (size_t i = r.begin(); i != r.end(); i++)
    //         {
    //             for (auto &elem : *(res_vector[i]))
    //             {
    //                 std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //                 (*cur_res)[elem.first] += elem.second;
    //                 // delete elem.first;
    //             }
    //             // delete res_vector[i];
    //         }
    //         return cur_res;
    //     },

    //     [&](std::map<std::string, size_t> *first, std::map<std::string, size_t> *second) {
    //         std::cout << (first == second) << std::endl;
    //         for (auto &elem : *first)
    //         {
    //             std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //             (*second)[elem.first] += elem.second;
    //         }
    //         // delete first;
    //         return second;
    //     });

    Reducer rd(res_vector);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, res_vector.size()), rd);
    auto res = rd.res;

    if (!std::filesystem::exists(configs.out_A_path.parent_path()))
    {
        std::filesystem::create_directory(configs.out_A_path.parent_path());
    }

    std::cout << "---Should have finished parallel_reduce---" << std::endl;

    std::ofstream outfile;
    std::vector<std::pair<std::string, size_t>> sorted;

    outfile.open(configs.out_A_path);

    for (auto it = res->begin(), next_it = it; it != res->end(); it = next_it)
    {
        outfile << (*it).first << ": " << (*it).second << std::endl;
        sorted.push_back((*it));
        ++next_it;
        res->erase((*it).first);
    }
    // delete res;
    outfile.close();
    outfile.open(configs.out_N_path);
    std::sort(sorted.begin(), sorted.end(), [](const std::pair<std::string, size_t> p1, const std::pair<std::string, size_t> p2) -> bool {
        return (p1.second > p2.second) ||
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

// --------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char const *argv[])
{
    // return test_main(argc, argv);

    config configs;
    if (argc >= 2)
    {
        if (argc == 2)
        {
            read_configs(configs, argv[1]);
        }
        else
        {
            std::cout << "Error in program arguments\n";
            return 1;
        }
    }
    else
    {
        read_configs(configs);
    }

    // std::cout << configs.out_A_path << configs.out_N_path << configs.boundary << configs.data_path << std::endl;

    auto start_time = get_current_time_fenced();

    int NTOKEN = 100; //TEMP

    std::vector<std::map<std::string, size_t> *> res_vector;

    std::filesystem::recursive_directory_iterator fls;
    fls = std::filesystem::recursive_directory_iterator{configs.data_path};
    auto file_end = std::filesystem::end(fls);

    // auto dictconcat_functor = [&](std::map<std::string, size_t> *dict) {
    //     std::cout << "the length is:" << res_vector.size() << std::endl;
    //     res_vector.push_back(dict);
    // };

    tbb::filter_t<void, MarkedString> input{
        tbb::filter::serial_in_order,
        FilterRead(&fls, &file_end)};

    tbb::filter_t<MarkedString, std::vector<std::string *>> forward_extract{
        tbb::filter::parallel,
        ArchiveForwardFilter()};

    tbb::filter_t<std::vector<std::string *>, std::map<std::string, size_t> *> index{
        tbb::filter::parallel,
        IndexerFilter()};

    tbb::filter_t<std::map<std::string, size_t> *, void> output{
        tbb::filter::serial_in_order,
        OutputFilter(&res_vector)};

    tbb::filter_t<void, void> lifecycle = input & forward_extract & index & output;
    tbb::parallel_pipeline(configs.boundary, lifecycle);

    // ------------------------------------------------------------Old paralel_reduce
    // auto res = tbb::parallel_reduce(
    //     tbb::blocked_range<size_t>(0, res_vector.size()),
    //     new std::map<std::string, size_t>,

    //     [&](tbb::blocked_range<size_t> &r, std::map<std::string, size_t> *cur_res) {
    //         for (size_t i = r.begin(); i != r.end(); i++)
    //         {
    //             for (auto &elem : *(res_vector[i]))
    //             {
    //                 (*cur_res)[elem.first] += elem.second;
    //                 // delete elem.first;
    //             }
    //             // delete res_vector[i];
    //         }
    //         return cur_res;
    //     },

    //     [&](std::map<std::string, size_t> *first, std::map<std::string, size_t> *second) {
    //         std::cout<<(first==second)<<std::endl;
    //         for (auto &elem : *first)
    //         {
    //             (*second)[elem.first] += elem.second;
    //         }
    //         // delete first;
    //         return second;
    //     });
    // -------------------------------------------------------------------------------------------------------

    Reducer rd(res_vector);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, res_vector.size()), rd);

    auto finish_time = get_current_time_fenced();
    auto time = finish_time - start_time;
    std::cout << "Working time: " << to_ms(time)/1000 << " s" << std::endl;

    auto res = rd.res;

    // delete &res_vector;
    // std::cout << "---Should have finished parallel_reduce---" << std::endl;
    std::ofstream outfile;
    std::vector<std::pair<std::string, size_t>> sorted;
    if (!std::filesystem::exists(configs.out_A_path.parent_path()))
    {
        std::filesystem::create_directory(configs.out_A_path.parent_path());
    }

    outfile.open(configs.out_A_path);

    for (auto it = res->begin(), next_it = it; it != res->end(); it = next_it)
    {
        outfile << (*it).first << ": " << (*it).second << std::endl;
        sorted.push_back((*it));
        ++next_it;
        res->erase((*it).first);
    }
    // delete res;
    outfile.close();
    outfile.open(configs.out_N_path);
    std::sort(sorted.begin(), sorted.end(), [](const std::pair<std::string, size_t> p1, const std::pair<std::string, size_t> p2) -> bool {
        return (p1.second > p2.second) ||
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
