# Include Environment-based settings.
include(${CMAKE_CURRENT_DOURCE_DIR}../common_settings.txt)

# Create the executable.
add_executable(rm rm.cpp)

# Indicate that the executable needs metasim and rtlib library.
target_link_libraries( rm rtlib ${metasim_LIBRARY} )
