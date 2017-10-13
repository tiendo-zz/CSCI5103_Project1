#include "var.h"


//void *f(int arg)
void *f(int arg)
{
//	getcontext(&(thread_list.next->thread_TCB.context));

/*
	address_t* _pointer;
	_pointer = thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP];
	printf("in func, f = %li\n", f);
	printf("in func, thread arg = %li\n", &arg);
	printf("in func, thread rsp = %li\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP]);
	printf("in func, thread rdi = %li\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RDI]);
	printf("in func, thread rsi = %li\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSI]);
	printf("in func, thread rdx = %li\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RDX]);
	printf("in func, thread rbp = %li\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RBP]);

	address_t* _pointer2;
	_pointer2 = thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RBP];
	_pointer2++;
	printf("in func, rbp[1] = %li\n", *(_pointer2) );


	printf("in func, add = %li, pointer[0] = %li\n", _pointer, *(_pointer)); _pointer++;
	printf("in func, add = %li, pointer[1] = %li\n", _pointer, *(_pointer)); _pointer++;
	printf("in func, add = %li, pointer[2] = %li\n", _pointer, *(_pointer)); _pointer++;
	printf("in func, add = %li, pointer[3] = %li\n", _pointer, *(_pointer)); _pointer++;
	printf("in func, add = %li, pointer[4] = %li\n", _pointer, *(_pointer)); _pointer++;
	printf("in func, add = %li, pointer[5] = %li\n", _pointer, *(_pointer)); _pointer++;
*/
	
	int i=0;
	while (i < 5){
		++i;
		printf("in f (%d)\n",i);

		//if (i == 5)
			//setcontext(&thread_list.thread_TCB.context);
		usleep(SECOND);
	}
	return 0;
}

int main()
{
	int i=0;


	getcontext(&thread_list.thread_TCB.context);
	if (i==0)
	{
		i++;
		uthread_create(&f, 20 /*(void*)&i*/);
	}

	// The first node in the linked list is always the main thread
	if (i == 1)
	{
		i++;
		printf("i = %i\n", i);
		printf("in main, thread sp = %li\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP]);
		address_t* _pointer2;
		_pointer2 = thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP];
		printf("in main, rsp = %li, rsp[0] = %li\n", _pointer2, *(_pointer2) );
		_pointer2++;
		printf("in main, rsp = %li, rsp[1] = %li\n", _pointer2, *(_pointer2) );
		_pointer2++;
		printf("in main, rsp = %li, rsp[2] = %li\n", _pointer2, *(_pointer2) );
		_pointer2++;
		printf("in main, rsp = %li, rsp[3] = %li\n", _pointer2, *(_pointer2) );

		setcontext(&(thread_list.next->thread_TCB.context));
	}


	printf("back to main\n");
	return 0;
}
