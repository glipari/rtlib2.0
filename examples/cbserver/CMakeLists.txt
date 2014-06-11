# Include Environment-based settings.
include(${CMAKE_CURRENT_DOURCE_DIR}../common_settings.txt)

# Create the executable.
add_executable(cbs cbs.cpp)

# Indicate that the executable needs metasim and rtlib library.
target_link_libraries( cbs rtlib ${metasim_LIBRARY} )
