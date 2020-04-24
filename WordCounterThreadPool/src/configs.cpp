#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <boost/algorithm/string.hpp>
// #include <boost/algorithm/trim.hpp>
#include "configs.hpp"

int read_configs(config &configs, std::string path)
{
    try {
        // boost::format("hello, %d" %d);
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
            if (words[0].find("max") != std::string::npos)
            {
                configs.max_threads = std::thread::hardware_concurrency() + std::stoi(words[1]);
                if (configs.max_threads < 1)
                {
                    throw std::logic_error{"The number of threads is too small. Please specify a number such max_threads >= 1."};
                }
            }
            else if (words[0].find("bound") != std::string::npos)
                configs.boundary = std::stoi(words[1]);
            else if (words[0].find("data") != std::string::npos)
                configs.data_path =  std::filesystem::path{boost::trim_copy(std::string{words[1]})};
            else if (words[0].find("A") != std::string::npos)
                configs.out_A_path = std::filesystem::path{boost::trim_copy(std::string{words[1]})};
            else if (words[0].find("N") != std::string::npos)
                configs.out_N_path = std::filesystem::path{boost::trim_copy(std::string{words[1]})};
        }
        file.close();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

}
