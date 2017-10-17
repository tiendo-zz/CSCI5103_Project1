include_directories(include)
include_directories(../ThreadScheduler/include)
add_library(uthread_new src/uthread.cpp)
target_link_libraries (uthread_new thread_scheduler)