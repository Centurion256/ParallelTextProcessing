cmake_minimum_required(VERSION 3.0.0)
project(WordCounter VERSION 0.1.0 LANGUAGES CXX) 

SET (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(LIBRARY_DIR "${CMAKE_SOURCE_DIR}/lib")

find_package(Threads REQUIRED)
include_directories("${CMAKE_SOURCE_DIR}/inc")
add_library(file_lib SHARED "${CMAKE_SOURCE_DIR}/inc/recursiveDirs.hpp" "${LIBRARY_DIR}/recursiveDirs.cpp")

add_executable(WordCounter "${CMAKE_SOURCE_DIR}/src/main.cpp")

target_link_libraries(WordCounter Threads::Threads file_lib)