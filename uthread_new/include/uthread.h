#ifndef _UTHREAD_
#define _UTHREAD_

#include "ThreadScheduler.h"


#define MAX_THREADS 1000
unsigned int thread_number;
ThreadScheduler* thread_scheduler;

int uthread_create(void (*start_routine)(int), int arg);


#endif