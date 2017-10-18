#include "uthread.h"
#include "assert.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define MAX_THREADS 1000
unsigned int TOTAL_THREAD_NUMBER;
unsigned int TIME_SLICE = 10000; // default 10ms, user can change value using uthread_init()
ThreadScheduler* thread_scheduler;

int uthread_create(void (*start_routine)(int), int arg){
  
  // disable timer interrupt
  thread_scheduler->DisableInterrupt();

  if(TOTAL_THREAD_NUMBER == 0){
    thread_scheduler = new ThreadScheduler();
    
    // Add the main thread
    TCB* main_thread = new TCB(TOTAL_THREAD_NUMBER++);
    thread_scheduler->AddThread(main_thread);
    
    // Add another thread    
    
    TCB* new_thread = new TCB(TOTAL_THREAD_NUMBER++); //, start_routine, arg, (size_t) stub);    
    thread_scheduler->AddThread(new_thread);
    getcontext(&thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->_context);
    thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->assign_context(start_routine, arg, (size_t) stub);    
     
  } else if(TOTAL_THREAD_NUMBER == MAX_THREADS){
    assert(TOTAL_THREAD_NUMBER == MAX_THREADS && "thread number reachs max. value");
  } else {
    // Add new thread
    // TCB* new_thread = new TCB(TOTAL_THREAD_NUMBER++, start_routine, arg, (size_t) stub);
    // thread_scheduler->AddThread(new_thread);
    
    std::cout << "_running_thread_id: " << thread_scheduler->_running_thread_id << std::endl;
    std::cout << "TOTAL_THREAD_NUMBER: " << TOTAL_THREAD_NUMBER << std::endl;
    TCB* new_thread = new TCB(TOTAL_THREAD_NUMBER++);    
    thread_scheduler->AddThread(new_thread);
    getcontext(&thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->_context);
    thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->assign_context(start_routine, arg, (size_t) stub);
    thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->print_context();
  }
  /*
  // To get main function context
  if(TOTAL_THREAD_NUMBER == 2){
    std::cout << "getting main context" << std::endl;
    thread_scheduler->GetMainContext();
  }*/

  //enable timer interrupt
  thread_scheduler->EnableInterrupt(TIME_SLICE);
}

/*
void sigalrm_handler_getmain(int sig)
{
  ucontext handlercontext;
  getcontext(&handlercontext);
  size_t* _ptr;
  _ptr = (size_t*) handlercontext.uc_mcontext.gregs[REG_RBP];
  
  std::cout << "ptr[0] - current sp " << *(_ptr) << std::endl;
  std::cout << "ptr[-1] - flags " << *(_ptr-1) << std::endl; 
  std::cout << "ptr[-2] - IP in main " << *(_ptr-2) << std::endl;
  std::cout << "ptr[-3] - SP in main " << *(_ptr-3) << std::endl;   


  // _ptr--;  
  ucontext maincontext;
  getcontext(&maincontext);
  maincontext.uc_mcontext.gregs[REG_RSP] = *(_ptr); _ptr--;
  maincontext.uc_mcontext.gregs[REG_RIP] = *(_ptr);    
  sigemptyset(&(maincontext.uc_sigmask));
  thread_scheduler->_vector_tcb[0]->_context = maincontext;

  std::cout << "main context ip: " << maincontext.uc_mcontext.gregs[REG_RIP] <<
      " sp: " << maincontext.uc_mcontext.gregs[REG_RSP] << std::endl;

  
  signal(SIGALRM, &sigalrm_handler_timeslice);
  alarm(1);
}
*/

int uthread_yield(void) {
  thread_scheduler->DisableInterrupt();

  // can we call the interrupt handler???
  sigalrm_handler_timeslice(1);

  thread_scheduler->EnableInterrupt(TIME_SLICE);
}


int uthread_self(void) {
  int tid;

  thread_scheduler->DisableInterrupt();

  tid = thread_scheduler->_running_thread_id;

  thread_scheduler->EnableInterrupt(TIME_SLICE);
  
  return tid;
}


int uthread_join(int tid, void **retval) {
  thread_scheduler->DisableInterrupt();

  int err_code = 0;

  // check if this "tid" exists
  if (tid >= TOTAL_THREAD_NUMBER)
    err_code = 1;
  else {
    // wait until thread is terminated
    while (thread_scheduler->_vector_tcb[tid]->get_state() != TERMINATED);
  }

  retval = thread_scheduler->_vector_tcb[tid]->get_retval();

  thread_scheduler->EnableInterrupt(TIME_SLICE);
  
  return err_code;
}


void uthread_exit(void *retval) {
  thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->set_state(TERMINATED);
  thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->set_retval(&retval);
}



int uthread_init(int time_slice) {
  TIME_SLICE = time_slice;
  return 0;
}


int uthread_terminate(int tid) {
  thread_scheduler->DisableInterrupt();

  int err_code = 0;

  // check if this "tid" exists
  if (tid >= TOTAL_THREAD_NUMBER)
    err_code = 1;
  else if (thread_scheduler->_vector_tcb[tid]->get_state() == TERMINATED) {
    std::cout << "thread already terminated\n";
    err_code = 2;	
  }
  else
    thread_scheduler->_vector_tcb[tid]->set_state(TERMINATED);

  thread_scheduler->EnableInterrupt(TIME_SLICE);

  return err_code;
}


int uthread_suspend(int tid) {
  thread_scheduler->DisableInterrupt();

  int err_code = 0;

  // check if this "tid" exists
  if (tid >= TOTAL_THREAD_NUMBER)
    err_code = 1;
  else if (thread_scheduler->_vector_tcb[tid]->get_state() == SUSPENDED) {
    std::cout << "thread already suspended\n";
    err_code = 2;	
  }
  else
    thread_scheduler->_vector_tcb[tid]->set_state(SUSPENDED);

  thread_scheduler->EnableInterrupt(TIME_SLICE);

  return err_code;
}



int uthread_resume(int tid) {
  thread_scheduler->DisableInterrupt();

  int err_code = 0;

  // check if this "tid" exists
  if (tid >= TOTAL_THREAD_NUMBER)
    err_code = 1;
  else if (thread_scheduler->_vector_tcb[tid]->get_state() == READY) {
    std::cout << "thread already running\n";
    err_code = 2;	
  }
  else
    thread_scheduler->_vector_tcb[tid]->set_state(READY);

  thread_scheduler->EnableInterrupt(TIME_SLICE);

  return err_code;
}

/*
ssize_t async_read(int fildes, void *buf, size_t nbytes) {
  thread_scheduler->DisableInterrupt();
  
  // set the O_ASYNC file status flag on the file descriptor
  fcntl(fildes, F_SETFL);

  struct sigaction sa;
  struct itimerval timer;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &async_IO;
  sigaction(SIGIO, &sa, NULL);  


  thread_scheduler->EnableInterrupt(TIME_SLICE);
  return 0;
}
*/


void sigalrm_handler_timeslice(int sig)
{
  int flag = 0;  
  ucontext handlercontext;
  getcontext(&handlercontext);
  
  if(flag == 1)
    return;

  
  thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->_context = handlercontext;  
  thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->set_state(READY);
  
  
  // simplest policy, but ignore if thread state is suspended or terminated
  while (1) {
	// choose next thread
	  if(thread_scheduler->_running_thread_id++ == TOTAL_THREAD_NUMBER-1){
	    thread_scheduler->_running_thread_id = 0;
	  }

	// if state is not READY, continue to another thread
  	if (thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->get_state() == READY)
	  break;
  }
  
  // thread switch    
  thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->set_state(RUNNING);  
  sigemptyset(&(thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->_context.uc_sigmask));


  //signal(SIGALRM, &sigalrm_handler_timeslice);
  //alarm(1);    
    
  flag = 1;
  setcontext(&(thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->_context));
}


void stub(void (*func)(int), int arg){  
  (*func)(arg);    
    
  int i = 0;  
  while(1){
    std::cout << "inside thread " << thread_scheduler->_running_thread_id << std::endl;
    usleep(5000000);
  }
}
