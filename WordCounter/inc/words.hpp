#include <iostream>
#include <boost/locale>
#include <map>
#include <string>
#include <mutex>
#include "words.hpp"

std::map splitWords(std::string text, std::map<std::string, int> words_count, std::mutex m);