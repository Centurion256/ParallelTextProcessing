
add_library(file_lib SHARED "${CMAKE_SOURCE_DIR}../inc/recursiveDirs.hpp" "${CMAKE_SOURCE_DIR}/recursiveDirs.cpp")
set_target_properties(file_lib
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}../bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}../bin"
)
target_link_libraries(file_lib archive_static archive)
