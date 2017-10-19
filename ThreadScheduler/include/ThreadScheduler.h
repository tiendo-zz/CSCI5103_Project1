#ifndef _THREAD_SCHEDULER_
#define _THREAD_SCHEDULER_

#include "ThreadControlBlock.h"
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <queue>


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
    void AddRunningQueue(TCB* new_thread);
    void AddSuspendedQueue(TCB* new_thread);
    void AddTerminatedQueue(TCB* new_thread);
    
    void EnableInterrupt(unsigned int time_slice);
    void DisableInterrupt();

    int find_thread_id(int tid);

    unsigned int _running_thread_id;        
    std::vector<TCB*> _vector_tcb;

    std::queue<TCB*> _running_queue;
    std::queue<TCB*> _suspended_queue;
    std::queue<TCB*> _terminated_queue;




};

int thread_switch(ThreadScheduler* ts, int num);
void sigalrm_handler_getmain(int sig);
void sigalrm_handler_timeslice(int sig);

#endif
