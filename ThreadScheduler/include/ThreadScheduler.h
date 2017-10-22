#ifndef _THREAD_SCHEDULER_
#define _THREAD_SCHEDULER_

#include "ThreadControlBlock.h"
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <queue>
#include <aio.h>
#include <map>

/** @class ThreadScheduler
 *  @brief Scheduling all threads 
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

    void AssignFileSize(int fileid, ssize_t filesize);
    
    void EnableInterrupt(unsigned int time_slice);
    void DisableInterrupt();
    void AssignFileIdToCurrentThread(int fileid);
    int GetThreadIdFromFileId(int fileid);
    
    int find_thread_id(int tid);

    unsigned int _running_thread_id;        
    std::vector<TCB*> _vector_tcb;

    std::queue<TCB*> _running_queue;
    std::queue<TCB*> _suspended_queue;

private:
    std::map<int, TCB*> _fildes_TCB;

};

void stub(void (*func)(void*), void *arg);
int thread_switch(ThreadScheduler* ts, int num);
void sigalrm_handler_getmain(int sig);
void sigalrm_handler_timeslice(int sig);
void aio_completion_handler( int signo, siginfo_t *info, void *context );

#endif
