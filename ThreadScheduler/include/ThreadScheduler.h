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
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

/** @class ThreadScheduler
 *  @brief Scheduling all threads using round-robin
 *   
 *  TODO: 
 */

class ThreadScheduler{
public:
    ThreadScheduler();

    ~ThreadScheduler();
    
    void AddThread(TCB* new_thread);
    void AddReadyQueue(TCB* new_thread);
    void AddTerminatedThread(TCB* new_thread);
    
        

    void AssignFileSize(int fileid, ssize_t filesize);
    void AssignFileIdToCurrentThread(int fileid);
    int GetThreadIdFromFileId(int fileid);
    
    
    
    // Once getting a context, make sure that it is stored, otherwise, repeated operation might happen
    int SwitchThread(int num);
    void EnableInterrupt(unsigned int time_slice);
    void DeleteThread(int tid);

    void DisableInterrupt();
    TCB* GetCurrentThread();
    // TCB* PopCurrentThread();
    TCB* FindThread(int tid);
    

    unsigned int _running_thread_id;        
    std::vector<TCB*> _vector_tcb;

    std::queue<TCB*> _ready_queue;    

private:
    std::map<int, TCB*> _fildes_TCB;    
    std::map<int, TCB*> _stored_retval;    
    std::queue<TCB*> _terminated_queue;
    struct sigaction* _sa;
    struct itimerval* _timer;
};

void stub(void (*func)(void*), void *arg);
void sigalrm_handler_getmain(int sig);
void sigalrm_handler_timeslice(int sig);
void aio_completion_handler( int signo, siginfo_t *info, void *context );

#endif
