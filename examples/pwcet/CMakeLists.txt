# Add include directories
include_directories(.)
include_directories(../../src)
include_directories(${metasim_INCLUDE_DIRS})
include_directories(${contrib_INCLUDE_DIRS})

set (EXECUTABLE_NAME example)
set (EXECUTABLE_SOURCES example.cpp)

# Create the executable.
add_executable (${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})
add_executable(pwcet pwcet.cpp)

target_compile_features (${EXECUTABLE_NAME} PRIVATE cxx_range_for)
target_compile_features (pwcet PRIVATE cxx_range_for)


# Indicate that the executable needs metasim library.
target_link_libraries (${EXECUTABLE_NAME} ${PROJECT_NAME} ${metasim_LIBRARIES} ${contrib_LIBRARIES})

target_link_libraries (pwcet ${PROJECT_NAME} ${metasim_LIBRARIES} ${contrib_LIBRARIES})
