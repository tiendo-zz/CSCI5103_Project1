#include "uthread.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



using namespace std;

void *foo(void *arg){
  cout << "Hello " << *((int*)arg) << endl; 
}




int main(){  
  ucontext maincontext;
  getcontext(&maincontext);

  uthread_init(100000);


  struct sigaction sa;
  struct itimerval timer;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &sigalrm_handler_timeslice;
  sigaction(SIGVTALRM, &sa, NULL);

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 10000; // 10ms = 10000 us
  timer.it_interval = timer.it_value;

  setitimer(ITIMER_VIRTUAL, &timer, NULL);



  cout << "main context ip: " << maincontext.uc_mcontext.gregs[REG_RIP] <<
      " sp: " << maincontext.uc_mcontext.gregs[REG_RSP] << endl;

  
  for(int i = 0; i < 10; i++){
    uthread_create(&foo, &i);
  }
  cout << "foo : " << (size_t) foo << endl;
  cout << "back in main" << endl;       
  int i = 0;

  uthread_suspend(3);
  uthread_resume(3);


  while(1){
    cout << "inside main ... " << ++i << endl;

    void **retval;

    uthread_join(10, retval);

    if (i == 5)
	uthread_suspend(5);
    if (i == 10) {
        uthread_resume(5);
        //uthread_resume(3);
    }
    usleep(500000);
  }




/*
  uthread_create(&foo, 1);

  int fd = open("./input.txt", O_RDONLY);
  char *buf;

  buf = (char*)malloc (100 * sizeof(char));

  //read(fd, buf, 100);
  async_read(fd, buf, 100);

  while (1);

  //cout << buf << endl;
*/
  
  return 0;
}
