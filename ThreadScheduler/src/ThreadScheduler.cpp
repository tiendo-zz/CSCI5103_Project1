#include "ThreadScheduler.h"


ThreadScheduler::ThreadScheduler()
{
  
}




ThreadScheduler::AddThread(TCB* new_thread)
{
  _vector_tcb.push_back(new_thread);
}
