include(FetchContent)
FetchContent_Declare(
  archlib
  GIT_REPOSITORY https://github.com/libarchive/libarchive.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(archlib)

set_target_properties(archive PROPERTIES
IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/build/_deps/archlib-build/bin/libarchive${CMAKE_SHARED_LIBRARY_SUFFIX}" 
) 

