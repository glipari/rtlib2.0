cmake_minimum_required (VERSION 3.1)
project (rtlib)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

set(LIBRARY_TYPE STATIC)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_definitions(-D__DEBUG__)
  message("Debug added as a compilation flag")
endif()

# Import MetaSim
find_package(metasim REQUIRED)
set(metasim_INCLUDE_DIRS "${metasim_DIR}/include")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  get_target_property(metasim_LIBRARIES metasim IMPORTED_LOCATION_RELEASE)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  get_target_property(metasim_LIBRARIES metasim IMPORTED_LOCATION_DEBUG)
endif()

message("Metasim configuration file directory: ${metasim_DIR}")
message("Metasim include directory: ${metasim_INCLUDE_DIRS}")
message("Metasim library: ${metasim_LIBRARIES}")

#include contrib
set(contrib_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/contrib/pwcet_simul/")
set(contrib_LIBRARIES "${PROJECT_SOURCE_DIR}/contrib/pwcet_simul/libpwcet.a")

message("contrib include dirs: ${contrib_INCLUDE_DIRS}")
message("contrib libraries: ${contrib_LIBRARIES}")

# Include dirs
add_subdirectory (src)
add_subdirectory (examples)

enable_testing (true)
add_subdirectory (test)

