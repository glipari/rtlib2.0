set(metasim_INCLUDE_DIRS "${metasim_DIR}/../src")

# Add defs for the compiler
if(APPLE)
	set(LIB_TYPE "SHARED")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")
	set(metasim_LIBRARY "${metasim_DIR}/src/libmetasim.dylib")
	
elseif(UNIX)
	set(LIB_TYPE "SHARED")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")
	set(metasim_LIBRARY "${metasim_DIR}/src/libmetasim.so")
endif()

# include dirs
include_directories(.)
include_directories(${metasim_INCLUDE_DIRS})
include_directories(../src)


# Create a library called "rtlib" which includes the source files.
add_executable(test test_main.cpp cbs.cpp)


# Indicate that rtlib need metasim library
target_link_libraries(test rtlib)
