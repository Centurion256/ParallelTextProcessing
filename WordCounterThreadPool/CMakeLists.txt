cmake_minimum_required(VERSION 3.14.0)
SET (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(WordCounter VERSION 0.1.0 LANGUAGES CXX) 

# set(WordCounter_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(LIBRARY_DIR "${CMAKE_SOURCE_DIR}/lib")

find_package(Threads REQUIRED)
include_directories("${CMAKE_SOURCE_DIR}/inc")

add_library(worker_lib SHARED "${CMAKE_SOURCE_DIR}/inc/workers.hpp" "${LIBRARY_DIR}/workers.cpp")
set_target_properties(worker_lib    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# add_library(words_lib SHARED "${CMAKE_SOURCE_DIR}/inc/words.hpp" "${LIBRARY_DIR}/words.cpp")
# set_target_properties(words_lib
#     PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
# )

add_library(thread_pool_lib SHARED "${CMAKE_SOURCE_DIR}/inc/ThreadPool.hpp" "${LIBRARY_DIR}/ThreadPool.cpp")
set_target_properties(thread_pool_lib
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

find_package(Boost 1.66.0 COMPONENTS locale REQUIRED)
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED       ON)  # use multithreaded
message("----------The include directories are: ${Boost_INCLUDE_DIRS}----------")
include_directories("${Boost_INCLUDE_DIRS}/boost/locale")
include_directories("${Boost_INCLUDE_DIRS}/boost/algorithm")

find_package(LibArchive)
IF (NOT LibArchive_FOUND)
# IF (UNIX)
    message("LibArchive not found in PATH. Installing a local copy of libarchive...")
    add_subdirectory("${CMAKE_SOURCE_DIR}/import")
# ELSEIF(WIN32)
#     message("On non-UNIX systems, libarchive must be installed manually.")
#     find_package(LibArchive REQUIRED)

ENDIF()

add_executable(WordCounter "${CMAKE_SOURCE_DIR}/src/main.cpp" "${CMAKE_SOURCE_DIR}/src/configs.cpp")
set_target_properties(WordCounter
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
IF (UNIX)
target_link_libraries(worker_lib archive thread_pool_lib Boost::locale)
ELSEIF (WIN32)
target_link_libraries(worker_lib LibArchive thread_pool_lib Boost::locale)
ENDIF()
# target_link_libraries(words_lib Boost::locale)
message("${Boost_LIBRARIES}")
target_link_libraries(WordCounter Threads::Threads thread_pool_lib worker_lib)
# words_lib queue_lib