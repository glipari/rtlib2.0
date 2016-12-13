# Add include directories
include_directories(.)
include_directories(../../src)
include_directories(${metasim_INCLUDE_DIRS})

set (EXECUTABLE_NAME spsl)
set (EXECUTABLE_SOURCES spsl.cpp)

# Create the executable.
add_executable (${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})

target_compile_features (${EXECUTABLE_NAME} PRIVATE cxx_range_for)

# Indicate that the executable needs metasim library.
target_link_libraries (${EXECUTABLE_NAME} ${PROJECT_NAME} ${metasim_LIBRARIES})

###############################

set (EXECUTABLE_NAME sprunt_example1)
set (EXECUTABLE_SOURCES sprunt_example1.cpp)

# Create the executable.
add_executable (${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})

target_compile_features (${EXECUTABLE_NAME} PRIVATE cxx_range_for)

# Indicate that the executable needs metasim library.
target_link_libraries (${EXECUTABLE_NAME} ${PROJECT_NAME} ${metasim_LIBRARIES})
