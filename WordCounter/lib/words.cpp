#include <iostream>
#include <boost/locale>
#include <map>
#include <string>
#include <mutex>
#include "words.hpp"

std::map splitWords(std::string text, std::mutex m)
{
    std::lock_guard<std::mutex> lg{m};
    std::map<std::string, int> words_count;
    boost::locale::boundary::ssegment_index words_list(boost::locale::boundary::word, text.begin(), text.end());
    words_list.map(boost::locale::boundary::word_any);
    for (boost::locale::boundary::ssegment_index::iterator it = words_list.begin(), e = words_list.end(); it != e; ++it)
    {
        std::cout << "\"" << *it << "\", ";
        ++words_count[*it];
    }
    return words_count;
}