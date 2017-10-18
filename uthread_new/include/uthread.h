#ifndef _UTHREAD_
#define _UTHREAD_

#include "ThreadScheduler.h"

int uthread_create(void (*start_routine)(int), int arg);
void stub(void (*func)(int), int arg);
void sigalrm_handler_getmain(int sig);

#endif
