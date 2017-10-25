# CSCI5103_Project1:

There are 2 tests describing how to use user thread library:
1/ ./Tester/test_uthread_new/test_uthread.cpp

2/ ./Tester/test_io_interrupt/test_async_read_func.cpp


Here is how to run the test:
Under this folder run the following command in terminal
$ mkdir build
$ cd build
$ cmake ..

-------------------------------------------------- TEST 1 --------------------------------------
To compile test 1 - test_uthread.cpp, under build directory we do:
$ cd Tester/test_uthread_new/
$ make test_uthread_simple

To run test 1 - test_uthread.cpp, we do:
$ ./test_uthread_simple


-------------------------------------------------- TEST 2 --------------------------------------
To compile test 2 - test_async_read_func.cpp, under build directory we do:
$ cd Tester/test_io_interrupt/
$ make test_asyn

To run test 2 - test_async_read_func.cpp, we do:
$ ./test_asyn



