#include "var.h"


void add_thread(struct TCB *thread_TCB);



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

/*
int uthread_create(void (*start_routine)(int), int arg)
{
	// allocate memory for new thread
	struct TCB *new_thread = malloc(sizeof(struct TCB));

	// allocate stack for new thread
	// char *stack = malloc(sizeof(char) * STACK_SIZE);
	unsigned int *stack = malloc(sizeof(unsigned int) * STACK_SIZE);

	// grab program counter
	// address_t pc = (address_t)stub;
	unsigned int pc = (unsigned int) stub;
	// address_t sp = (address_t)stack + STACK_SIZE;
	unsigned int sp = (unsigned int) stack + STACK_SIZE * sizeof(unsigned int);

	new_thread->thread_id = thread_counter++;

	// Get the current execution context
	getcontext(&new_thread->context);

	// Modify the context to a new stack
	new_thread->context.uc_mcontext.gregs[REG_RIP] = pc;
	// new_thread->context.uc_mcontext.gregs[REG_RSP] = sp - 2;
	sigemptyset(&new_thread->context.uc_sigmask);

	stack += STACK_SIZE;
	printf("stack addr = %i\n", stack);
	
	*(stack) =  arg;
	stack--;
	printf("stack addr = %i\n", stack);
	// stack -= sizeof(address_t);
	*(stack) =  start_routine;
	stack--;
	printf("stack addr = %i\n", stack);
	//stack -= sizeof(address_t);
	
	printf("RSP = %i\n", sp - 2 * sizeof(unsigned int));

	new_thread->context.uc_mcontext.gregs[REG_RSP] = sp - 2 * sizeof(unsigned int);
	// stack += STACK_SIZE;
	// printf("stack = %i\n", stack);

	// stack -= sizeof(int);
	// *stack = (address_t)start_routine;

	printf("start routine = %i\n", start_routine);

	new_thread->state = READY;

	add_thread(new_thread);
	
	return 0;
}


void stub(void (*func)(int), int arg)
{
	
	printf("in stub\n");	
	printf("func addr = %i\n", func);
	printf("arg addr = %i\n", &arg);
	printf("arg - func = %i\n", (unsigned int)(&arg) - (unsigned int)func);
	getcontext(&(thread_list.next->thread_TCB.context));
	printf("current sp = %i\n",thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP]);

	printf("arg - sp = %i\n", (unsigned int)(&arg) - (unsigned int)thread_list.next->thread_TCB.context.uc_mcontext.gregs[REG_RSP]);
	printf("arg = %i\n", arg);
	(*func)(arg);
	// **func(NULL);
}

*/

void add_thread(struct TCB *thread_TCB)
{
	struct Node *new_node = malloc(sizeof(struct Node));
	struct Node *last = &thread_list;

	new_node->thread_TCB = *thread_TCB;

	while (last->next != NULL)
		last = last->next;

	last->next = new_node;
}
