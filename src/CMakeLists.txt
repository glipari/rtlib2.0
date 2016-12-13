# Include dirs.
include_directories(.)
include_directories(${metasim_INCLUDE_DIRS})

# Create a library called "rtlib" which includes the source files.
add_library(${PROJECT_NAME} ${LIBRARY_TYPE} capacitytimer.cpp cbserver.cpp cpu.cpp
  edfsched.cpp exeinstr.cpp fcfsresmanager.cpp feedback.cpp feedbacktest.cpp 
  fifosched.cpp fpsched.cpp grubserver.cpp interrupt.cpp jtrace.cpp 
  kernel.cpp kernevt.cpp load.cpp mrtkernel.cpp piresman.cpp pollingserver.cpp 
  reginstr.cpp regsched.cpp regtask.cpp resmanager.cpp resource.cpp 
  rmsched.cpp rrsched.cpp rttask.cpp schedinstr.cpp schedpoints.cpp schedrta.cpp 
  scheduler.cpp server.cpp sparepot.cpp sporadicserver.cpp supercbs.cpp 
  task.cpp taskevt.cpp texttrace.cpp threinstr.cpp timer.cpp traceevent.cpp 
  tracepower.cpp waitinstr.cpp instr.cpp suspend_instr.cpp AVRTask.cpp json_trace.cpp)

target_link_libraries (${PROJECT_NAME} ${metasim_LIBRARIES})

target_compile_features (${PROJECT_NAME} PRIVATE cxx_range_for)

# Export.
export (TARGETS ${PROJECT_NAME} FILE "./rtlibConfig.cmake")
export (PACKAGE ${PROJECT_NAME})
