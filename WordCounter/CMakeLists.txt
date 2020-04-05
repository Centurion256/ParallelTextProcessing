cmake_minimum_required(VERSION 3.14.0)
SET (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(WordCounter VERSION 0.1.0 LANGUAGES CXX) 

# set(WordCounter_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(LIBRARY_DIR "${CMAKE_SOURCE_DIR}/lib")

find_package(Threads REQUIRED)
include_directories("${CMAKE_SOURCE_DIR}/inc")
add_library(file_lib SHARED "${CMAKE_SOURCE_DIR}/inc/recursiveDirs.hpp" "${LIBRARY_DIR}/recursiveDirs.cpp")
set_target_properties(file_lib
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_library(queue_lib SHARED "${CMAKE_SOURCE_DIR}/inc/queue.hpp" "${LIBRARY_DIR}/queue.cpp")
set_target_properties(queue_lib
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_library(words_lib SHARED "${CMAKE_SOURCE_DIR}/inc/words.hpp" "${LIBRARY_DIR}/words.cpp")
set_target_properties(words_lib
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

find_package(Boost REQUIRED)
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
include_directories(${Boost_INCLUDE_DIRS})

find_package(LibArchive)
IF (NOT LibArchive_FOUND)
# IF (UNIX)
    message("LibArchive not found in PATH. Installing a local copy of libarchive...")
    add_subdirectory("${CMAKE_SOURCE_DIR}/import")
# ELSEIF(WIN32)
#     message("On non-UNIX systems, libarchive must be installed manually.")
#     find_package(LibArchive REQUIRED)

ENDIF()

add_executable(WordCounter "${CMAKE_SOURCE_DIR}/src/main.cpp")
set_target_properties(WordCounter
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
target_link_libraries(file_lib LibArchive)

target_link_libraries(WordCounter Threads::Threads file_lib queue_lib words_lib LibArchive ${Boost_LIBRARIES})
