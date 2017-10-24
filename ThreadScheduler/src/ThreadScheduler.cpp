#include "ThreadScheduler.h"
#include <iostream>

using namespace std;

#define MAX_TERMINATED_QUEUE_SIZE 1000
// TODO: MAKE SURE TO BLOCK ALL INTERRUPTS



ThreadScheduler::ThreadScheduler()
{  
  _sa = new struct sigaction;   
  _timer = new struct itimerval;
}


void ThreadScheduler::AddThread(TCB* new_thread) {  
  _vector_tcb.push_back(new_thread);
}


void ThreadScheduler::AddReadyQueue(TCB* new_thread) {  
  _ready_queue.push(new_thread);
}

void ThreadScheduler::DeleteThread(int tid)
{
  vector<TCB*>::iterator it = _vector_tcb.begin();
  bool flag_found = 0;
  while (it != _vector_tcb.end()) {
    if ((*it)->get_thread_id() == tid){
      flag_found = 1;
      break;
    }
    it++;
  }
  if(flag_found){    
    delete (*it);
    _vector_tcb.erase(it);    
  }
}



void ThreadScheduler::AddTerminatedThread(TCB* thread) {        
  
  // Not very optimized but for the time being
  vector<TCB*>::iterator it = _vector_tcb.begin();
  bool flag_found = 0;
  while (it != _vector_tcb.end()) {
    if ((*it)->get_thread_id() == thread->get_thread_id()){
      flag_found = 1;
      break;
    }
    it++;
  }
  
  
  if(flag_found){
    if(_terminated_queue.size() == MAX_TERMINATED_QUEUE_SIZE){
      TCB* very_old_finished_thread = _terminated_queue.front();
      _terminated_queue.pop();
      delete very_old_finished_thread;
    }
    _terminated_queue.push(*it);
    _vector_tcb.erase(it);    
  }
}


TCB* ThreadScheduler::GetCurrentThread()
{
  return _ready_queue.front();
}

TCB* ThreadScheduler::FindThread(int tid)
{
  vector<TCB*>::iterator it = _vector_tcb.begin();
  bool flag_found = 0;
  while (it != _vector_tcb.end()) {
    if ((*it)->get_thread_id() == tid){
      flag_found = 1;
      break;
    }
    it++;
  }
  if(flag_found)
    return *(it);
  else
    return NULL;
}



void ThreadScheduler::EnableInterrupt(unsigned int time_slice)
{
  memset(_sa, 0, sizeof(_sa));
  _sa->sa_handler = &sigalrm_handler_timeslice;
  sigaction(SIGALRM, _sa, NULL);

  _timer->it_value.tv_sec = 0;
  _timer->it_value.tv_usec = time_slice;
  _timer->it_interval = _timer->it_value;

  setitimer(ITIMER_REAL, _timer, NULL);
} 


void ThreadScheduler::DisableInterrupt()
{
  _timer->it_value.tv_sec = 0;
  _timer->it_value.tv_usec = 0;
  _timer->it_interval = _timer->it_value;
  setitimer(ITIMER_REAL, _timer, NULL);
} 


void ThreadScheduler::AssignFileIdToCurrentThread(int fileid)
{ 
  _fildes_TCB[fileid] = _ready_queue.front();
}


void ThreadScheduler::AssignFileSize(int fileid, ssize_t filesize)
{
  std::map<int, TCB*>::iterator it;
  it = _fildes_TCB.find(fileid);
  it->second->_file_size = filesize;
}

int ThreadScheduler::GetThreadIdFromFileId(int fileid)
{
  std::map<int, TCB*>::iterator it;
  it = _fildes_TCB.find(fileid);
  return it->second->get_thread_id();
}


int ThreadScheduler::SwitchThread(int num)
{
  
}













