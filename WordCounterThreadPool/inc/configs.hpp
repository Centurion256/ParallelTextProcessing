#ifndef CONFIGS
#define CONFIGS

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <filesystem>


struct config
{
    int max_threads;
    int boundary;
    std::filesystem::path data_path;
    std::filesystem::path out_A_path;
    std::filesystem::path out_N_path;
};

int read_configs(config &configs, std::string path="example.conf");
#endif