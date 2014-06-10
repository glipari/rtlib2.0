set(metasim_INCLUDE_DIRS "${metasim_DIR}/../src")
set(metasim_LIBRARY "${metasim_DIR}/src/Debug/metasim.lib")

# Add defs for the compiler
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")
endif()
# MSVC does not require any special flags

# include dirs
include_directories(.)
include_directories(${metasim_INCLUDE_DIRS})
include_directories(../../src)


# Create the executable.
# The extension is already found. Any number of sources could be listed here.
if(WIN32)
add_executable(edf edf.cpp)
elseif(UNIX)
add_executable(edf edf.cpp)
else()
message( FATAL_ERROR "Unsupported environment (!?)." )
endif()

# Indicate that the executable needs metasim and rtlib library
target_link_libraries( edf rtlib )
