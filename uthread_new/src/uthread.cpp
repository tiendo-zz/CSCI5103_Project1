#include "uthread.h"
#include "assert.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

using namespace std;

#define MAX_THREADS 1000
#define MIN_TIME_SLICE 1000 // 1msec

static unsigned int THREAD_ID = 0;
static unsigned int TOTAL_THREAD_NUMBER = 0; // this variable must be atomic, when this changes, interript must be disabled
unsigned int TIME_SLICE = 10000; // default 10ms, user can change value using uthread_init()
ThreadScheduler* thread_scheduler;

int uthread_create(void *(*start_routine)(void*), void *arg){   

  if(THREAD_ID == 0){
    thread_scheduler = new ThreadScheduler();
    
    // Add the main thread
    TCB* main_thread = new TCB(THREAD_ID++); TOTAL_THREAD_NUMBER++;

    // set main thread state to running
    main_thread->set_state(RUNNING);

    thread_scheduler->AddThread(main_thread);
    thread_scheduler->AddReadyQueue(main_thread);
    
    // Add another thread    
    
    TCB* new_thread = new TCB(THREAD_ID++); TOTAL_THREAD_NUMBER++;
    thread_scheduler->AddThread(new_thread);
    thread_scheduler->AddReadyQueue(new_thread);

    getcontext(&thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->_context);
    thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->assign_context(start_routine, arg, (size_t) stub);    
    
    thread_scheduler->EnableInterrupt(TIME_SLICE);
  } else {
    if(TOTAL_THREAD_NUMBER == MAX_THREADS){
    // assert(TOTAL_THREAD_NUMBER == MAX_THREADS && "thread number reachs max. value");
    return -1;
  } else {    
    // disable timer interrupt
    // BUG: if a thread keeps calling thread create, no other can run, but MAX thread will partially fix it
    thread_scheduler->DisableInterrupt();
    
    // Add new thread    
    TCB* new_thread = new TCB(THREAD_ID++);    TOTAL_THREAD_NUMBER++;
    new_thread->print_context();
    thread_scheduler->AddThread(new_thread);
    thread_scheduler->AddReadyQueue(new_thread);    
    
    
    getcontext(&thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->_context);
    thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->assign_context(start_routine, arg, (size_t) stub);
    
    // enable timer interrupt
    thread_scheduler->EnableInterrupt(TIME_SLICE);    
  }
  }
}


int uthread_yield(void) {
  thread_scheduler->DisableInterrupt();

  // can we call the interrupt handler???
  sigalrm_handler_timeslice(1);

  thread_scheduler->EnableInterrupt(TIME_SLICE);
  
  return 0;
}


int uthread_self(void) {  
  return thread_scheduler->GetCurrentThread()->get_thread_id();
}


// IMPORTANT NOTE: ONLY UTHREAD_JOIN REMOVE THE THREAD TCB FROM _VECTOR_TCB
int uthread_join(int tid, void **retval) {  
  if (tid == 0) { // cannot join "main" thread    
    return -1;
  }
  
  TCB* joining_thread = thread_scheduler->FindThread(tid);
  
  // cannot find thread_id in vector_tcb means this thread_id does not exist or already terminated
  if (joining_thread == NULL) {
    return -1;
  }
  else { // found thread in vector_tcb, needs to wait until thread is terminated
    
      // BUG: what if somehow joining_thread is deleted by the thread yield --> need assumption
      while (joining_thread && joining_thread->get_state() != TERMINATED) {
        uthread_yield(); // while it waits, other threads can still run
        usleep(500); // spinning a while hopefully it's finished before switch thread
      }
      
      if(joining_thread){
        thread_scheduler->DisableInterrupt();
        
        // BUG: If interrupt happens right here and somehow it delete this thread --> need assumption
        retval = joining_thread->get_retval();
      
        // the thread is terminated and the return value has assigned
        // now we do not need this thread ever so we delete it immediately
        thread_scheduler->DeleteThread(tid);
        TOTAL_THREAD_NUMBER--;
//        joining_thread->set_state(FINISHED);
                
        thread_scheduler->EnableInterrupt(TIME_SLICE);
        return 0;
      } else {        
        return -1;
      }
  }  
}


// this functions is put at the end of stub function to terminate thread
void uthread_exit(void *retval) {  
  TCB* running_thread = thread_scheduler->GetCurrentThread();

  // set the state to TERMINATED
  // thread scheduler will remove it from running_queue
  running_thread->set_state(TERMINATED);
  running_thread->set_retval(&retval);

  // thread terminated, let other threads continue
  uthread_yield();  
}



int uthread_init(int time_slice) {
  if(time_slice < MIN_TIME_SLICE)
    return -1;
  
  TIME_SLICE = time_slice;
  return 0;
}


int uthread_terminate(int tid) {  
  int err_code = 0;

  if (tid == 0) { // cannot terminate "main" thread
    err_code = -1;
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
    err_code = -1;
  else {
    (*it)->set_state(TERMINATED);
    
    // is the thread terminating itself?
    TCB* running_thread = thread_scheduler->GetCurrentThread();
    if (running_thread->get_thread_id() == tid)
      uthread_yield();
  }  
  return err_code;
}


int uthread_suspend(int tid) {  
  int err_code = 0;

  if (tid == 0) { // cannot suspend "main" thread
    err_code = -1;
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
    // just to check if thread is already terminated
    // if user hasn't called uthread_join, terminated thread_TCB is still on vector_tcb
    if (((*it)->get_state() != TERMINATED) && ((*it)->get_state() != SUSPENDED))  {
      (*it)->set_state(SUSPENDED);
    }

    // is the thread suspending itself?
    TCB* running_thread = thread_scheduler->GetCurrentThread();
    if (running_thread->get_thread_id() == tid)
      uthread_yield();
  }  
  return err_code;
}



int uthread_resume(int tid) {  

  int err_code = 0;
  int i;

  if (tid == 0) { // cannot resume "main" thread
    err_code = -1;
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
    err_code = -1;
  }
  else { // only need to change state, thread scheduler will put it back to RunningQueue
    if ((*it)->get_state() == SUSPENDED)
      (*it)->set_state(READY);
  }  

  return err_code;
}


void aio_completion_handler( int signo, siginfo_t *info, void *context ){    
  struct aiocb *req;  
  
  /* Ensure it's our signal */
  if (info->si_signo == SIGIO) {    
    req = (struct aiocb *)info->si_value.sival_ptr;
 
    /* Did the request complete? */
    if (aio_error( req ) == 0) {            
      
      /* Request completed successfully, get the return status */
      int file_size = aio_return( req );
      thread_scheduler->AssignFileSize(req->aio_fildes, file_size);
      uthread_resume(thread_scheduler->GetThreadIdFromFileId(req->aio_fildes));      
    }
  }     
  return;
}

ssize_t async_read(int fildes, void *buf, size_t nbytes){
  ssize_t file_finish = 0;
  
  int ret;  
  struct sigaction sig_act;  
  struct aiocb my_aiocb;
  
  /* Set up the signal handler */
  sigemptyset(&sig_act.sa_mask);
  sig_act.sa_flags = SA_SIGINFO;
  sig_act.sa_sigaction = aio_completion_handler; 
 
  /* Set up the AIO request */
  bzero( (char *)&my_aiocb, sizeof(struct aiocb) );
  my_aiocb.aio_fildes = fildes;
  my_aiocb.aio_buf = buf;
  my_aiocb.aio_nbytes = nbytes;
  my_aiocb.aio_offset = 0;
 
  /* Link the AIO request with the Signal Handler */
  my_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  my_aiocb.aio_sigevent.sigev_signo = SIGIO;
  my_aiocb.aio_sigevent.sigev_value.sival_ptr = &my_aiocb;
 
  /* Map the Signal to the Signal Handler */
  ret = sigaction( SIGIO, &sig_act, NULL );  
  ret = aio_read( &my_aiocb );  
  
  // Assign file descriptor to the thread_id  
  thread_scheduler->AssignFileIdToCurrentThread(fildes);
  
  // suspend my thread immediately
  int current_thread_id = thread_scheduler->GetCurrentThread()->get_thread_id();
  uthread_suspend(current_thread_id);

  return thread_scheduler->GetCurrentThread()->_file_size;
}

void sigalrm_handler_timeslice(int sig)
{
  // flag in the stack to infer that the context has been safely stored
  bool flag = 0;  
  ucontext handlercontext;
  getcontext(&handlercontext);
  
  if(flag == 1)
    return;

  thread_scheduler->DisableInterrupt(); 
 
  // current running thread
  TCB* current_thread = thread_scheduler->GetCurrentThread();
  thread_scheduler->_ready_queue.pop();  
  current_thread->assign_context(handlercontext);
  current_thread->print_context();
  // has user changed the state of this thread?
  if (current_thread->get_state() == RUNNING) {
    current_thread->set_state(READY);
    // push running thread to back of the queue
    thread_scheduler->AddReadyQueue(current_thread);
  }

  // grab the next READY thread
  int i = 0;
  TCB* next_thread = thread_scheduler->GetCurrentThread();
  next_thread->print_context();
  // iterate until getting the one is ready
  while (next_thread->get_state() != READY) {
    thread_scheduler->_ready_queue.pop();
    
    if(next_thread->get_state() == TERMINATED){ // here threads are those whose their thread exit is already called
      thread_scheduler->AddTerminatedThread(next_thread);
      TOTAL_THREAD_NUMBER--; // The interript is already disabled, so we're good.
    } /*else if (next_thread->get_state() == FINISHED) {
      thread_scheduler->DeleteThread(next_thread->get_thread_id());
      TOTAL_THREAD_NUMBER--; // The interript is already disabled, so we're good.
    }*/ else {
      thread_scheduler->AddReadyQueue(next_thread);
    }
    next_thread = thread_scheduler->GetCurrentThread();
    next_thread->print_context();
  }
    
  next_thread->set_state(RUNNING);
  
  sigemptyset(&(next_thread->_context.uc_sigmask));          
  
  flag = 1;

  thread_scheduler->EnableInterrupt(TIME_SLICE);
  
  setcontext(&(next_thread->_context));
}


void stub(void (*func)(void*), void *arg){  
  (*func)(arg);
  uthread_exit(0);
}
