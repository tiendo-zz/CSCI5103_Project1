include_directories(../../uthread_new/include/)
include_directories(../../ThreadScheduler/include)
add_executable(test_uthread_simple test_uthread.cpp)
target_link_libraries (test_uthread_simple uthread_new -lrt)
