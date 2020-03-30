cmake_minimum_required(VERSION 3.0.0)
project(WordCounter VERSION 0.1.0 LANGUAGES CXX) 

SET (CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_library(file_lib SHARED recursiveDirs.hpp recursiveDirs.cpp)

add_executable(WordCounter main.cpp)

target_link_libraries(WordCounter Threads::Threads file_lib)