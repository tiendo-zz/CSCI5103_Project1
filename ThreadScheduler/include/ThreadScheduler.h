#ifndef _THREAD_SCHEDULER_
#define _THREAD_SCHEDULER_

#include "ThreadControlBlock.h"
#include <vector>
#include <signal.h>
#include <unistd.h>

/** @class ThreadScheduler
 *  @brief Scheduling all threads 
 *   
 *  @input:  sensor or observer data
 *  @output: setpoint assignment to controller
 *   
 *  TODO: 
 */


class ThreadScheduler{
public:
    ThreadScheduler();

    ~ThreadScheduler();
    
    void AddThread(TCB* new_thread);
    
    void GetMainContext();

    unsigned int _running_thread_id;        
    std::vector<TCB*> _vector_tcb;
};

int thread_switch(ThreadScheduler* ts, int num);
void sigalrm_handler_getmain(int sig);
void sigalrm_handler_timeslice(int sig);

#endif