#ifndef _THREAD_CONTROL_BLOCK_
#define _THREAD_CONTROL_BLOCK_

#include "ucontext.h"
#include <functional>
#include <iostream>
#include <unistd.h>

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
  TCB(int thread_id):_thread_id(thread_id) {}
  
  TCB(int thread_id, ucontext context):_thread_id(thread_id),
                                       _context(context),
                                       _state(READY){}
  
  TCB(int thread_id, void *(*start_routine)(void*), void *arg, size_t stub_pc):_thread_id(thread_id),    
                                                                          _state(READY),
                                                                          _arg(arg){

    char *stack = new char[STACK_SIZE];            

    size_t sp = (size_t) (stack + STACK_SIZE - 2*sizeof(size_t)/sizeof(char));
    
    _context.uc_mcontext.gregs[REG_RIP] = stub_pc;
    _context.uc_mcontext.gregs[REG_RSP] = sp;
    sigemptyset(&_context.uc_sigmask);
    _context.uc_mcontext.gregs[REG_RDI] = (size_t) start_routine;
    _context.uc_mcontext.gregs[REG_RSI] = (size_t) arg;
  }
  
  // setter
  void assign_context(ucontext context){
    _context = context;
  }
  
  void assign_context(void *(*start_routine)(void*), void *arg, size_t stub_pc){
    char *stack = new char[STACK_SIZE];                
    size_t sp = (size_t) (stack + STACK_SIZE - 2*sizeof(size_t)/sizeof(char));
    _context.uc_mcontext.gregs[REG_RIP] = stub_pc;
    _context.uc_mcontext.gregs[REG_RSP] = sp;    
    sigemptyset(&_context.uc_sigmask);
    _context.uc_mcontext.gregs[REG_RDI] = (size_t) start_routine;
    _context.uc_mcontext.gregs[REG_RSI] = (size_t) arg;
  }
  
  void set_state(thread_state state){
    _state = state;
  }
  
  // getter
  ucontext get_context(){
    return this->_context;
  }
  
  thread_state get_state(){
    return this->_state;
  }
  
  // debugger
  void print_context(){
      std::cout << "TCB id: " << _thread_id 
                << " state: " << _state 
                << " ip: " << _context.uc_mcontext.gregs[REG_RIP] 
                << " sp: " << _context.uc_mcontext.gregs[REG_RSP] 
                << " di: " << _context.uc_mcontext.gregs[REG_RDI] 
                << " si: " << _context.uc_mcontext.gregs[REG_RSI] 
                << std::endl;
  }

  int get_thread_id() {
    return this->_thread_id;
  }
  
  // back to where it left
  void thread_resume(){
    std::cout << "starting to run stub" << std::endl;
    setcontext(&this->_context);
  }

  void set_retval(void **retval) {
   _retval = (void**)retval;
  }

  void** get_retval() {
    return this->_retval;
  }
  
  
  ~TCB();
  
  
  ucontext _context;
  ssize_t _file_size;
protected:
  int _thread_id;
  
  enum thread_state _state;
  void *_arg;
  void** _retval;
};




#endif
