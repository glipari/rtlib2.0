cmake_minimum_required (VERSION 3.1)
project (rtlib)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set(LIBRARY_TYPE STATIC)

# Import MetaSim
find_package(metasim REQUIRED)
set(metasim_INCLUDE_DIRS "${metasim_DIR}/include")
get_target_property(metasim_CONFIGURATION metasim IMPORTED_CONFIGURATIONS)
if (${metasim_CONFIGURATION} STREQUAL "RELEASE")
  get_target_property(metasim_LIBRARIES metasim IMPORTED_LOCATION_RELEASE)
elseif (${metasim_CONFIGURATION} STREQUAL "DEBUG")
  get_target_property(metasim_LIBRARIES metasim IMPORTED_LOCATION_DEBUG)
endif()

# Include dirs
add_subdirectory (src)
add_subdirectory (examples)

enable_testing (true)
add_subdirectory (test)
