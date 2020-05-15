#ifndef RECURSIVE_DIRS_LIB
#define RECURSIVE_DIRS_LIB

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <future>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include "archive.h"
#include "archive_entry.h"


#include "ThreadPool.hpp"

void worker_read(ThreadPool<std::map<std::string, int>*>& pool, std::string path, std::promise<int>&& res);
void worker_extract(ThreadPool<std::map<std::string, int>*>& pool, std::string* file_str);
void worker_index(ThreadPool<std::map<std::string, int>*>& pool, std::string* file_str);
void worker_merge(ThreadPool<std::map<std::string, int>*>& pool, std::map<std::string, int>* first, std::map<std::string, int>* second);

#endif