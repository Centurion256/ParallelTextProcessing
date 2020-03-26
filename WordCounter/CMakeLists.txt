cmake_minimum_required(VERSION 3.0.0)
project(WordCounter VERSION 0.1.0 LANGUAGES CXX) 

include(CTest)
enable_testing()

add_executable(WordCounter main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})