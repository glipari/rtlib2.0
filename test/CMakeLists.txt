# Add include directories
include_directories (.)
include_directories (../src)
include_directories(${metasim_INCLUDE_DIRS})

set (RTLIB_TEST_LIBRARY ${PROJECT_NAME}_test)

# Create Catch library to decrease compile times
add_library (${RTLIB_TEST_LIBRARY} ${LIBRARY_TYPE} TestMain.cpp)

# Define a macro to simplify tests creation
macro (create_test name src)
    add_executable (${name} ${src})
    target_compile_features (${name} PRIVATE cxx_range_for)
    target_link_libraries (${name} ${RTLIB_TEST_LIBRARY} ${PROJECT_NAME} ${metasim_LIBRARIES})
    add_test (NAME ${name} COMMAND ${name})
endmacro (create_test)

create_test (TestCBS cbs.cpp)
create_test (TestTask test_task.cpp)
create_test (TestMRT test_mrt.cpp)
create_test (TestAVR test_AVR.cpp)
create_test (TestThresholds test_thresholds.cpp)
