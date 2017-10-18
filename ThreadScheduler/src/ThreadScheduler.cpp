#include "ThreadScheduler.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>


// TODO: MAKE SURE TO BLOCK ALL INTERRUPTS

ThreadScheduler::ThreadScheduler()
{
  
}




void ThreadScheduler::AddThread(TCB* new_thread)
{  
  _vector_tcb.push_back(new_thread);
}


void ThreadScheduler::GetMainContext()
{
  //signal(SIGALRM, &sigalrm_handler_timeslice);
  //alarm(1);  

  struct sigaction sa;
  struct itimerval timer;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &sigalrm_handler_timeslice;
  sigaction(SIGALRM, &sa, NULL);

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 10000; // 10ms = 10000 us
  timer.it_interval = timer.it_value;

  setitimer(ITIMER_REAL, &timer, NULL);
} 
