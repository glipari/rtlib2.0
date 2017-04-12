# Add include directories
include_directories (.)
include_directories (../src)
include_directories(${metasim_INCLUDE_DIRS})

set (RTLIB_TEST_LIBRARY ${PROJECT_NAME}_test)

# Create Catch library to decrease compile times
add_library (${RTLIB_TEST_LIBRARY} ${LIBRARY_TYPE} TestMain.cpp)

# Define a macro to simplify tests creation
macro (create_test name)
    target_compile_features (${name} PRIVATE cxx_range_for)
    target_link_libraries (${name} ${RTLIB_TEST_LIBRARY} ${PROJECT_NAME} ${metasim_LIBRARIES})
    add_test (NAME ${name} COMMAND ${name})
endmacro (create_test)

add_executable (TestCBS test_CBS.cpp)
create_test (TestCBS)

add_executable (TestTask test_task.cpp)
create_test (TestTask)

add_executable (TestMRT test_mrt.cpp)
create_test (TestMRT)

add_executable (TestAVR test_AVR.cpp)
create_test (TestAVR)

add_executable (TestThresholds test_thresholds.cpp)
create_test (TestThresholds)
