#include "ThreadScheduler.h"
#include <iostream>

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
  signal(SIGALRM, &sigalrm_handler_timeslice);
  alarm(1);  
} 