cmake_minimum_required (VERSION 2.6)
project (UserThread)


set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
set(WARNING_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-local-typedefs -Wno-reorder")
set(COMPILATION_FLAGS "${WARNING_FLAGS} -fexceptions -frtti -std=c++11 -fmodulo-sched -fmodulo-sched-allow-regmoves -O3")
set(COMMON_OPTIMIZATION_FLAGS "-ftree-loop-im -funswitch-loops -floop-interchange")
set(DEBUG_FLAGS "-fexceptions -std=c++11 -O1 -g3")

# libraries
add_subdirectory(uthread)


# test folders
add_subdirectory(test_uthread)
