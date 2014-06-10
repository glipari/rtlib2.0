set(metasim_INCLUDE_DIRS "${metasim_DIR}/../src")
set(metasim_LIBRARY "${metasim_DIR}/src/Debug/metasim.lib")

# Add defs for the compiler
if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")
  set(metasim_LIBRARY "${metasim_DIR}/src/libmetasim.a")
else()
  set(metasim_LIBRARY "${metasim_DIR}/src/Debug/metasim.lib")
endif()
# MSVC does not require any special flags

# include dirs
include_directories(.)
include_directories(${metasim_INCLUDE_DIRS})
include_directories(../../src)


# Create a library called "rtlib" which includes the source files.
# The extension is already found. Any number of sources could be listed here.
add_executable(test test_main.cpp cbs.cpp)


# Indicate that rtlib need metasim library
target_link_libraries(test rtlib)
