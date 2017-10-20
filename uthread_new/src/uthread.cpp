#include "uthread.h"
#include "assert.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

using namespace std;

#define MAX_THREADS 1000
unsigned int TOTAL_THREAD_NUMBER;
unsigned int TIME_SLICE = 100000; // default 10ms, user can change value using uthread_init()
ThreadScheduler* thread_scheduler;

int uthread_create(void (*start_routine)(int), int arg){
  
  // disable timer interrupt
  thread_scheduler->DisableInterrupt();

  if(TOTAL_THREAD_NUMBER == 0){
    thread_scheduler = new ThreadScheduler();
    
    // Add the main thread
    TCB* main_thread = new TCB(TOTAL_THREAD_NUMBER++);

    // set main thread state to running
    main_thread->set_state(RUNNING);

    thread_scheduler->AddThread(main_thread);
    thread_scheduler->AddRunningQueue(main_thread);
    
    // Add another thread    
    
    TCB* new_thread = new TCB(TOTAL_THREAD_NUMBER++); //, start_routine, arg, (size_t) stub);    
    thread_scheduler->AddThread(new_thread);
    thread_scheduler->AddRunningQueue(new_thread);

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
    thread_scheduler->AddRunningQueue(new_thread);

    getcontext(&thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->_context);
    thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->assign_context(start_routine, arg, (size_t) stub);
    thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->print_context();
  }


  //enable timer interrupt
  thread_scheduler->EnableInterrupt(TIME_SLICE);
}




int uthread_yield(void) {
  thread_scheduler->DisableInterrupt();

  // can we call the interrupt handler???
  sigalrm_handler_timeslice(1);

  thread_scheduler->EnableInterrupt(TIME_SLICE);
}


int uthread_self(void) {
  int tid;

  thread_scheduler->DisableInterrupt();

  // grab info of current thread
  TCB* running_thread = thread_scheduler->_running_queue.front();

  tid = running_thread->get_thread_id();

  thread_scheduler->EnableInterrupt(TIME_SLICE);
  
  return tid;
}


// IMPORTANT NOTE: ONLY UTHREAD_JOIN REMOVE THE THREAD TCB FROM _VECTOR_TCB
int uthread_join(int tid, void **retval) {
  thread_scheduler->DisableInterrupt();

  int err_code = 0;


  if (tid == 0) { // cannot join "main" thread
    err_code = 1;
    return err_code;
  }

  vector<TCB*>::iterator it = thread_scheduler->_vector_tcb.begin();

  while (it != thread_scheduler->_vector_tcb.end()) {
    if ((*it)->get_thread_id() == tid)
      break;
    it++;
  }

  // cannot find thread_id in vector_tcb means this thread_id does not exist or already terminated
  if (it == thread_scheduler->_vector_tcb.end()) {
    err_code = 1;
  }
  else { // found thread in vector_tcb, needs to wait until thread is terminated
      while ((*it)->get_state() != TERMINATED) {
        cout << "main waiting for 10 to terminate\n";
        uthread_yield(); // while it waits, other threads can still run
      }

      retval = (*it)->get_retval();
      // remove terminated thread from vector_tcb
      thread_scheduler->_vector_tcb.erase(it);
  }


  thread_scheduler->EnableInterrupt(TIME_SLICE);
  
  return err_code;
}


// user is not allowed to call this function
void uthread_exit(void *retval) {
  thread_scheduler->DisableInterrupt();

  TCB* running_thread = thread_scheduler->_running_queue.front();

  // set the state to TERMINATED
  // thread scheduler will remove it from running_queue
  running_thread->set_state(TERMINATED);
  running_thread->set_retval(&retval);

  // thread terminated, let other threads continue
  uthread_yield();

  thread_scheduler->EnableInterrupt(TIME_SLICE);
}



int uthread_init(int time_slice) {
  TIME_SLICE = time_slice;
  return 0;
}


int uthread_terminate(int tid) {
  thread_scheduler->DisableInterrupt();

  int err_code = 0;

  if (tid == 0) { // cannot terminate "main" thread
    err_code = 1;
    return err_code;
  }

  // find thread_id in _vector_tcb
  vector<TCB*>::iterator it = thread_scheduler->_vector_tcb.begin();

  while (it != thread_scheduler->_vector_tcb.end()) {
    if ((*it)->get_thread_id() == tid)
      break;
    it++;
  }

  // cannot find thread_id in vector_tcb
  // thread_id is already terminated or doesn't exist
  if (it == thread_scheduler->_vector_tcb.end())
    err_code = 1;
  else {
    (*it)->set_state(TERMINATED);
    
    // is the thread terminating itself?
    TCB* running_thread = thread_scheduler->_running_queue.front();
    if (running_thread->get_thread_id() == tid)
      uthread_yield();
  }

  thread_scheduler->EnableInterrupt(TIME_SLICE);

  return err_code;
}


int uthread_suspend(int tid) {
  thread_scheduler->DisableInterrupt();

  int err_code = 0;

  if (tid == 0) { // cannot suspend "main" thread
    err_code = 1;
    return err_code;
  }

  // find thread_id in _vector_tcb
  vector<TCB*>::iterator it = thread_scheduler->_vector_tcb.begin();

  while (it != thread_scheduler->_vector_tcb.end()) {
    if ((*it)->get_thread_id() == tid)
      break;
    it++;
  }

  // cannot find thread_id in vector_tcb
  // thread_id is already terminated or doesn't exist
  if (it == thread_scheduler->_vector_tcb.end()) {
    err_code = 1;
  }
  else {
    (*it)->set_state(SUSPENDED);

    // is the thread suspending itself?
    TCB* running_thread = thread_scheduler->_running_queue.front();
    if (running_thread->get_thread_id() == tid)
      uthread_yield();
  }

  thread_scheduler->EnableInterrupt(TIME_SLICE);

  return err_code;
}



int uthread_resume(int tid) {
  thread_scheduler->DisableInterrupt();

  int err_code = 0;
  int i;

  if (tid == 0) { // cannot resume "main" thread
    err_code = 1;
    return err_code;
  }

  TCB* resume_thread;

  // find thread_id in suspended_queue
  for (i = 0; i < thread_scheduler->_suspended_queue.size(); i++) {
    resume_thread = thread_scheduler->_suspended_queue.front();
    thread_scheduler->_suspended_queue.pop();

    // is this the thread we want to resume?
    if (resume_thread->get_thread_id() == tid) {
      resume_thread->set_state(READY);
      thread_scheduler->AddRunningQueue(resume_thread);
      err_code = 0;
      break;
    }
    else { // put it back to end of _suspended_queue
      thread_scheduler->AddSuspendedQueue(resume_thread);
      err_code = 1;
    }
  }

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

  thread_scheduler->DisableInterrupt(); 
 
  // current running thread
  TCB* current_thread = thread_scheduler->_running_queue.front();
  thread_scheduler->_running_queue.pop();
  
  // has user changed the state of this thread?
  if (current_thread->get_state() == RUNNING) {
    current_thread->_context = handlercontext;
    current_thread->set_state(READY);
    // push running thread to back of the queue
    thread_scheduler->AddRunningQueue(current_thread);
  }
  else if (current_thread->get_state() == SUSPENDED) {
    thread_scheduler->AddSuspendedQueue(current_thread);
  }


  // grab the next READY thread
  TCB* next_thread = thread_scheduler->_running_queue.front();

  while (next_thread->get_state() != READY) {
    thread_scheduler->_running_queue.pop();

    if (next_thread->get_state() == SUSPENDED) {
      thread_scheduler->AddSuspendedQueue(next_thread);
    }

    next_thread = thread_scheduler->_running_queue.front();
  }

  next_thread->set_state(RUNNING);
  sigemptyset(&(next_thread->_context.uc_sigmask));

    
  flag = 1;

  thread_scheduler->EnableInterrupt(TIME_SLICE);

  //setcontext(&(thread_scheduler->_vector_tcb[thread_scheduler->_running_thread_id]->_context));
  setcontext(&(next_thread->_context));
}


void stub(void (*func)(int), int arg){  
  (*func)(arg);    
    
  int i = 0;  
  while(1){
    i++;
    //std::cout << "inside thread " << thread_scheduler->_running_thread_id << std::endl;
    TCB* current_thread = thread_scheduler->_running_queue.front();
    int tid = current_thread->get_thread_id();

    std::cout << "inside thread " << tid << std::endl;
    
    if (tid == 9 && i == 10)
      uthread_terminate(10);

    //if (tid == 10)
      //uthread_exit(0);

    usleep(500000);
  }
}
