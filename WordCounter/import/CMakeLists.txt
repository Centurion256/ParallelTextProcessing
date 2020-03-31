include(FetchContent)
FetchContent_Declare(
  archlib
  GIT_REPOSITORY https://github.com/libarchive/libarchive.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(archlib)