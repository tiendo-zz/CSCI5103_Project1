#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define __USE_GNU
#include <ucontext.h>

#include <signal.h>

#define TIME_SLICE 10000  // 10 msec
#define SECOND 1000000
#define STACK_SIZE 4096


// function prototype
//int uthread_create(void (*start_routine)(int), int arg);
int uthread_create(void *(*start_routine)(void*), void *arg);
int uthread_yield(void);
int uthread_self(void);
int uthread_join(int tid, void **retval);

//void *f(int arg);
void *f(void *arg);
void stub(void (*func)(int), int arg);




// structure definition
typedef unsigned long address_t;

enum thread_state {
	READY,
	RUNNING,
	SUSPENDED,
	TERMINATED
};



struct TCB {
	int 			thread_id;
	ucontext_t 		context;
	enum thread_state	state;
	void			*arg;
	void			**retval;
};


struct Node {
	struct TCB thread_TCB;
	struct Node *next;
};
