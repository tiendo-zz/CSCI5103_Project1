#ifndef _UTHREAD_
#define _UTHREAD_

#include "ThreadScheduler.h"

int uthread_create(void *(*start_routine)(void*), void *arg);
int uthread_yield(void);
int uthread_self(void);
int uthread_join(int tid, void **retval);
void uthread_exit(void *retval);


int uthread_init(int time_slice);
int uthread_terminate(int tid);
int uthread_suspend(int tid);
int uthread_resume(int tid);


ssize_t async_read(int fildes, void *buf, size_t nbytes);

#endif
