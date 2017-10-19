#include "ThreadScheduler.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

using namespace std;


// TODO: MAKE SURE TO BLOCK ALL INTERRUPTS

ThreadScheduler::ThreadScheduler()
{
  
}




void ThreadScheduler::AddThread(TCB* new_thread) {  
  _vector_tcb.push_back(new_thread);
}



void ThreadScheduler::AddRunningQueue(TCB* new_thread) {  
  _running_queue.push(new_thread);
}


void ThreadScheduler::AddSuspendedQueue(TCB* new_thread) {  
  _suspended_queue.push(new_thread);
}


void ThreadScheduler::AddTerminatedQueue(TCB* new_thread) {  
  _terminated_queue.push(new_thread);
}


int ThreadScheduler::find_thread_id(int tid) {
  int i;

  for (i = 0; i < _vector_tcb.size(); i++)
    if (_vector_tcb[i]->get_thread_id() == tid)
      break;

  return i;
}



void ThreadScheduler::EnableInterrupt(unsigned int time_slice)
{
  //signal(SIGALRM, &sigalrm_handler_timeslice);
  //alarm(1);  

  struct sigaction sa;
  struct itimerval timer;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &sigalrm_handler_timeslice;
  sigaction(SIGALRM, &sa, NULL);

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = time_slice;
  timer.it_interval = timer.it_value;

  setitimer(ITIMER_REAL, &timer, NULL);
} 


void ThreadScheduler::DisableInterrupt()
{
  //signal(SIGALRM, &sigalrm_handler_timeslice);
  //alarm(1);  

  struct sigaction sa;
  struct itimerval timer;

  //memset(&sa, 0, sizeof(sa));
  //sa.sa_handler = &sigalrm_handler_timeslice;
  //sigaction(SIGALRM, &sa, NULL);

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 0;
  timer.it_interval = timer.it_value;

  setitimer(ITIMER_REAL, &timer, NULL);
} 
