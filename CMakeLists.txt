cmake_minimum_required (VERSION 2.6)
project (rtlib)

# packages
find_package(metasim REQUIRED)

# include dirs
add_subdirectory (src)
add_subdirectory (examples)
add_subdirectory (test)

#export
export(TARGETS rtlib FILE "./rtlibConfig.cmake")
export(PACKAGE rtlib)
