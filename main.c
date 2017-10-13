#include "var.h"


//void *f(int arg)
void *f(void *arg)
{
	int i=0;
	

	getcontext(&(thread_list.next->thread_TCB.context));
	printf("in func, thread sp = %i\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP]);

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
		uthread_create(&f, (void*)&i);
	}

	// The first node in the linked list is always the main thread
	if (i == 1)
	{
		i++;
		printf("i = %i\n", i);
		printf("in main, thread sp = %i\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP]);
		setcontext(&(thread_list.next->thread_TCB.context));
	}


	printf("back to main\n");
	return 0;
}
