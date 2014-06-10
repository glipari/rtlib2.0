set(metasim_INCLUDE_DIRS "${metasim_DIR}/../src")

# Add defs for the compiler
if(APPLE)
	set(LIB_TYPE "SHARED")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")
	set(metasim_LIBRARY "${metasim_DIR}/src/libmetasim.dylib")
	
elseif(UNIX)
	set(LIB_TYPE "SHARED")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11")
	set(metasim_LIBRARY "${metasim_DIR}/src/libmetasim.so")
endif()

# include dirs
include_directories(.)
include_directories(${metasim_INCLUDE_DIRS})


# Create a library called "rtlib" which includes the source files.
add_library(rtlib ${LIB_TYPE} bwi.cpp capacitytimer.cpp cbserver.cpp cpu.cpp 
  edfsched.cpp exeinstr.cpp fcfsresmanager.cpp feedback.cpp feedbacktest.cpp 
  fifosched.cpp fpsched.cpp grubserver.cpp interrupt.cpp jtrace.cpp 
  kernel.cpp kernevt.cpp load.cpp mrtkernel.cpp piresman.cpp pollingserver.cpp 
  reginstr.cpp regsched.cpp regtask.cpp resmanager.cpp resource.cpp 
  rmsched.cpp rrsched.cpp schedinstr.cpp schedpoints.cpp schedrta.cpp 
  scheduler.cpp server.cpp sparepot.cpp sporadicserver.cpp supercbs.cpp 
  task.cpp taskevt.cpp texttrace.cpp threinstr.cpp timer.cpp traceevent.cpp 
  waitinstr.cpp instr.cpp)

# Indicate that rtlib need metasim library
target_link_libraries( rtlib  ${metasim_LIBRARY} )
