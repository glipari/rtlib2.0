# Include Environment-based settings.
include(${CMAKE_CURRENT_DOURCE_DIR}../common_settings.txt)

# Create the executable.
add_executable(spsl spsl.cpp)
add_executable(sprunt_example1 sprunt_example1.cpp)

# Indicate that the executable needs metasim and rtlib library.
target_link_libraries( spsl rtlib )
target_link_libraries( sprunt_example1 rtlib ${metasim_LIBRARY} )
