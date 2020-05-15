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
    add_subdirectory("${CMAKE_SOURCE_DIR}/import/boost")
# ELSEIF(WIN32)
#     message("On non-UNIX systems, libarchive must be installed manually.")
#     find_package(LibArchive REQUIRED)
ENDIF()

find_package(TBB)
IF (NOT TBB_FOUND)
    message("Intel-TBB not found in PATH. Installing a local copy of Intel-TBB...")
    include("cmake/TBBGet.cmake")
    tbb_get(TBB_ROOT tbb_root_dir CONFIG_DIR tbb_config)
    set(TBB_FIND_RELEASE_ONLY TRUE)
    find_package(TBB REQUIRED tbb)
    # add_subdirectory("${CMAKE_SOURCE_DIR}/import/tbb")
ENDIF()

add_executable(WordCounter "${CMAKE_SOURCE_DIR}/src/main.cpp" "${CMAKE_SOURCE_DIR}/src/configs.cpp")
set_target_properties(WordCounter
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

#REDUNDANT
IF (UNIX)
target_link_libraries(worker_lib archive Boost::locale)
ELSEIF (WIN32)
target_link_libraries(worker_lib LibArchive Boost::locale)
ENDIF()
# target_link_libraries(words_lib Boost::locale)
message("${Boost_LIBRARIES}")
IF (UNIX)
target_link_libraries(WordCounter Threads::Threads LibArchive TBB::tbb)
ELSEIF (WIN32)
target_link_libraries(WordCounter Threads::Threads libarchive TBB::tbb)
ENDIF()
# words_lib queue_lib