#include "var.h"


void add_thread(struct TCB *thread_TCB);


/*
int uthread_create(void *(*start_routine)(void*), void *arg)
{
	// allocate memory for new thread
	struct TCB *new_thread = malloc(sizeof(struct TCB));

	// allocate stack for new thread
	char *stack = malloc(sizeof(char) * STACK_SIZE);

	// grab program counter
	address_t pc = (address_t)(**start_routine);
	address_t sp = (address_t)stack + STACK_SIZE - sizeof(int);

	new_thread->thread_id = thread_counter++;

	// Get the current execution context
	getcontext(&new_thread->context);

	// Modify the context to a new stack
	new_thread->context.uc_mcontext.gregs[REG_RIP] = pc;
	new_thread->context.uc_mcontext.gregs[REG_RSP] = sp;
	sigemptyset(&new_thread->context.uc_sigmask);

	new_thread->context.uc_stack.ss_sp = stack;
	new_thread->context.uc_stack.ss_size = sizeof(stack);
	new_thread->context.uc_link = &(thread_list.thread_TCB.context);

	new_thread->state = READY;

	add_thread(new_thread);
	
	return 0;
}
*/

int uthread_create(void (*start_routine)(int), int arg)
{
	// allocate memory for new thread
	struct TCB *new_thread = malloc(sizeof(struct TCB));

	// allocate stack for new thread
	// char *stack = malloc(sizeof(char) * STACK_SIZE);
	char *stack = malloc(sizeof(char) * STACK_SIZE);

	// grab program counter
	// address_t pc = (address_t)stub;
	address_t pc = (address_t) stub;
	// address_t sp = (address_t)stack + STACK_SIZE;
	address_t sp = (address_t) (stack + STACK_SIZE - 2*sizeof(address_t)/sizeof(char));

	new_thread->thread_id = thread_counter++;

	// Get the current execution context
	getcontext(&new_thread->context);

	// Modify the context to a new stack
	new_thread->context.uc_mcontext.gregs[REG_RIP] = pc;
	new_thread->context.uc_mcontext.gregs[REG_RSP] = sp;
	sigemptyset(&new_thread->context.uc_sigmask);

	stack += STACK_SIZE;
	printf("stack addr = %i\n", stack);
	
	*(stack) = arg;	
	stack-=sizeof(address_t)/sizeof(char);
	printf("stack addr = %i\n", stack);


	// ((address_t) stack[0]) = start_routine;
	
	address_t val = start_routine;	
	memcpy(stack, &(val), 8);
	stack-=sizeof(address_t)/sizeof(char);
	printf("stack addr = %i\n", stack);

	
	printf("RSP = %i\n", sp);

	new_thread->context.uc_mcontext.gregs[REG_RDI] = (address_t) start_routine;
	new_thread->context.uc_mcontext.gregs[REG_RSI] = arg;


	printf("start routine = %i\n", start_routine);

	new_thread->state = READY;

	add_thread(new_thread);
	
	return 0;
}


void stub(void (*func)(int), int arg)
{
	

	printf("in stub, thread rsp = %li\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP]);

	address_t* _pointer2;
	_pointer2 = thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP];
	
	printf("in stub, rsp = %li, rsp[0] = %li\n", _pointer2, *(_pointer2) );
	_pointer2++;
	printf("in stub, rsp = %li, rsp[1] = %li\n", _pointer2, *(_pointer2) );
	_pointer2++;
	printf("in stub, rsp = %li, rsp[2] = %li\n", _pointer2, *(_pointer2) );
	_pointer2++;
	printf("in stub, rsp = %li, rsp[3] = %li\n", _pointer2, *(_pointer2) );

	printf("in stub, rdi = %li\n", thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RDI]);

	(*func)(arg);
	
	printf("end stub \n");
}

void add_thread(struct TCB *thread_TCB)
{
	struct Node *new_node = malloc(sizeof(struct Node));
	struct Node *last = &thread_list;

	new_node->thread_TCB = *thread_TCB;

	while (last->next != NULL)
		last = last->next;

	last->next = new_node;
}
