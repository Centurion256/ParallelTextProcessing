#ifndef WORDS_LIB_HEADER
#define WORDS_LIB_HEADER

#include <iostream>
#include <boost/locale.hpp>
#include <map>
#include <string>
#include <mutex>
#include "words.hpp"

void parse_file(std::map<std::string, int>* words_count, std::string* text);

#endif