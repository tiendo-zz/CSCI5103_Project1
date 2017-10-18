#include "uthread.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>



using namespace std;

void foo(int arg){
  cout << "Hello " << arg << endl; 
}




int main(){  
  ucontext maincontext;
  getcontext(&maincontext);

/*
  struct sigaction sa;
  struct itimerval timer;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &sigalrm_handler_timeslice;
  sigaction(SIGVTALRM, &sa, NULL);

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 10000; // 10ms = 10000 us
  timer.it_interval = timer.it_value;

  setitimer(ITIMER_VIRTUAL, &timer, NULL);
*/


  cout << "main context ip: " << maincontext.uc_mcontext.gregs[REG_RIP] <<
      " sp: " << maincontext.uc_mcontext.gregs[REG_RSP] << endl;

  
  for(int i = 0; i < 10; i++){
    uthread_create(&foo, i);
  }
  cout << "foo : " << (size_t) foo << endl;
  cout << "back in main" << endl;       
  int i = 0;
  while(1){
    cout << "inside main ... " << ++i << endl;
    usleep(5000000);
  }
  
  return 0;
}
