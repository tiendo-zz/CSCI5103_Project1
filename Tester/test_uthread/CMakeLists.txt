cmake_minimum_required(VERSION 2.6)
include_directories(../uthread/include)


add_executable(test_uthread main.c)
target_link_libraries (test_uthread uthread)