#ifndef _THREAD_SCHEDULER_
#define _THREAD_SCHEDULER_

#include "ThreadControlBlock.h"
#include <vector>

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
    AddThread();
private:
    std::vector<TCB*> _vector_tcb;
};


#endif