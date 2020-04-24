#include <iostream>
// #include <locale>
#include <boost/locale.hpp>
#include <map>
#include <string>
#include <mutex>
#include "words.hpp"



void parse_file(std::map<std::string, int> *words_count, std::string *text)
{
    using namespace boost::locale::boundary;
    boost::locale::generator gen;
    std::locale loc = gen("");
    ssegment_index words_list(word, text->begin(), text->end(),loc);
    // std::cout<<"Error in index\n";
    words_list.rule(word_letters);
    std::string key;
    // std::cout << "Entered boost\n";
    for (ssegment_index::iterator it = words_list.begin(), e = words_list.end(); it != e; ++it)
    // for (auto elem:words_list)
    {
        // std::cout << "I'm here" << std::endl;
        // std::cout << "The item is: " << it.dereference() << std::endl;
        // std::cout << "\"" << elem << "\", ";
        key = boost::locale::fold_case(boost::locale::normalize((*it).str(), boost::locale::norm_type::norm_default, loc), loc);
        // std::cout<<"Derefer iter is "<<key<<"\n";
        ++(*words_count)[key];
    }


}