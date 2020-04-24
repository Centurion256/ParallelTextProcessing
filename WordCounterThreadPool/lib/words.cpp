#include <iostream>
// #include <locale>
#include <boost/locale.hpp>
#include <map>
#include <string>
#include <mutex>
#include "words.hpp"



void parse_file(std::map<std::string, int> *words_count, std::string *text)
{

    // boost::locale::generator gen;
    // std::locale l = gen("");
    // std::locale::global(l);
    // std::cout.imbue(l);
    // std::cout << boost::locale::as::date << std::time(0) << std::endl;


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

    // std::cout << "Entered parse_file..." << std::endl;
    // boost::locale::boundary::ssegment_index words_list(boost::locale::boundary::word, (*text).begin(), (*text).end());
    // std::cout<<"Error not in ssegment_index" << std::endl;
    // words_list.rule(boost::locale::boundary::word_any);
    // std::cout << "Error not in rule" << std::endl;
    // for (boost::locale::boundary::ssegment_index::iterator it = words_list.begin(), e = words_list.end(); it != e; ++it)
    // {
    //     std::cout << "\"" << *it << "\", ";
    //     std::cout<<"Derefer iter is "<<it.dereference()<<"\n";
    //     ++(*words_count)[it.dereference()];
    // }
//     using namespace boost::locale::boundary;
// boost::locale::generator gen;
// std::string text="To be or not to be, that is the question."
// // Create mapping of text for token iterator using global locale.
// ssegment_index map(word,text.begin(),text.end(),gen("en_US.UTF-8")); 
// // Print all "words" -- chunks of word boundary
// for(ssegment_index::iterator it=map.begin(),e=map.end();it!=e;++it)
//     std::cout <<"\""<< * it << "\", ";
// std::cout << std::endl;
}