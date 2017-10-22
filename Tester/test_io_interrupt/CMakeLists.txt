include_directories(../../uthread_new/include/)
include_directories(../../ThreadScheduler/include)


add_executable(test_io test_io_interrupt.cpp)
target_link_libraries (test_io uthread_new -lrt)

add_executable(test_asyn test_async_read_func.cpp)
target_link_libraries (test_asyn uthread_new -lrt)