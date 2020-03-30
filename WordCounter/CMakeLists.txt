cmake_minimum_required(VERSION 3.0.0)
project(WordCounter VERSION 0.1.0 LANGUAGES CXX) 
SET (CMAKE_C_COMPILER_WORKS 1)
SET (CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_CXX_STANDARD 17)

set(FILE_LIB_NAME file_lib)
add_library(${FILE_LIB_NAME} SHARED recursiveDirs.hpp recursiveDirs.cpp)

set(EXEC_NAME WordCounter)
add_executable(${EXEC_NAME} main.cpp)
target_link_libraries(${EXEC_NAME} ${FILE_LIB_NAME})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})