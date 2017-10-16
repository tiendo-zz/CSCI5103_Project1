#include "uthread.h"
#include "assert.h"

int uthread_create(void (*start_routine)(void*), void* arg){
  
  if(thread_number == 0){
    thread_scheduler = new ThreadScheduler();
    
    // Add the main thread
    TCB* main_thread = new TCB(thread_number++, NULL);
    thread_scheduler->AddThread(main_thread);
    
    // Add another thread    
    TCB* new_thread = new TCB(thread_number++, start_routine, arg);
    thread_scheduler->AddThread(new_thread);
    
  } else if(thread_number == MAX_THREADS){
    assert(thread_number == MAX_THREADS >= 0 && "thread number reachs max. value");
  } else {
    // Add new thread
    TCB* new_thread = new TCB(thread_number++, start_routine, arg);
    thread_scheduler->AddThread();
  }
  
}
