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
unsigned int TIME_SLICE = 10000; // default 10ms, user can change value using uthread_init()
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
    // thread_scheduler->_vector_tcb[TOTAL_THREAD_NUMBER-1]->print_context();
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


// this functions is put at the end of stub function to terminate thread
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
    // just to check if thread is already terminated
    // if user hasn't called uthread_join, terminated thread_TCB is still on vector_tcb
    if (((*it)->get_state() != TERMINATED) && ((*it)->get_state() != SUSPENDED))  {
      (*it)->set_state(SUSPENDED);
      thread_scheduler->AddSuspendedQueue(*it);
    }

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
  int current_thread_id = thread_scheduler->_running_queue.front()->get_thread_id();
  uthread_suspend(current_thread_id);

  return thread_scheduler->_running_queue.front()->_file_size;
}

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
  
  current_thread->_context = handlercontext;

  // has user changed the state of this thread?
  if (current_thread->get_state() == RUNNING) {
    current_thread->set_state(READY);
    // push running thread to back of the queue
    thread_scheduler->AddRunningQueue(current_thread);
  }



  // grab the next READY thread
  TCB* next_thread = thread_scheduler->_running_queue.front();

  while (next_thread->get_state() != READY) {
    thread_scheduler->_running_queue.pop();

    next_thread = thread_scheduler->_running_queue.front();
  }

  next_thread->set_state(RUNNING);
  sigemptyset(&(next_thread->_context.uc_sigmask));

    
  flag = 1;

  // this is to check if any suspended thread is terminated
  for (int i = 0; i < thread_scheduler->_suspended_queue.size(); i++) {
    TCB* temp_thread = thread_scheduler->_suspended_queue.front();
    thread_scheduler->_suspended_queue.pop();

    // if it is still suspended, put it back to the queue
    // if terminated, just remove it completely
    // uthread_join will remove the terminated thread from vector_tcb
    if (temp_thread->get_state() == SUSPENDED)
      thread_scheduler->AddSuspendedQueue(temp_thread);
  }

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
