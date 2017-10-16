#ifndef _THREAD_CONTROL_BLOCK_
#define _THREAD_CONTROL_BLOCK_

#include "ucontext.h"

/** @class TCB
 *  @brief 
 *   
 *  @input:  
 *  @output: 
 *   
 *  TODO: 
 */


#define STACK_SIZE 4096

enum thread_state {
  READY,
  RUNNING,
  SUSPENDED,
  TERMINATED
};

class TCB{
public:
    TCB(int thread_id, ucontext context){
    _thread_id = thread_id;
    _context = context;
    _state = READY;
  }

  TCB(int thread_id, void (*start_routine)(void*), void* arg){
    _thread_id = thread_id;    
    _state = READY;
    _arg = arg;
    
    char *stack = new char[STACK_SIZE];        
    size_t pc = (size_t) this->stub;
    size_t sp = (size_t) (stack + STACK_SIZE);
    
    _context.uc_mcontext.gregs[REG_RIP] = pc;
    _context.uc_mcontext.gregs[REG_RSP] = sp;
    sigemptyset(_context.uc_sigmask);
    _context.uc_mcontext.gregs[REG_RDI] = (size_t) start_routine;
    _context.uc_mcontext.gregs[REG_RSI] = arg;
  }
  
  
  ~TCB();

protected:
  int _thread_id;
  ucontext _context;
  enum thread_state _state;
  void* _arg;
  void** _retval;
  
  
  
  void stub(void (*func)(void*), void* arg)
  {
    (*func)(arg);    
  }

};




#endif