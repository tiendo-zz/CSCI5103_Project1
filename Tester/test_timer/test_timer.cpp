#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <ucontext.h>


using namespace std;

// global vars
int i = 0;
ucontext_t maincontext, handlercontext;

void sigalrm_handler(int sig){
  cout << "inside handler" << endl;
  ++i;
  
  getcontext(&handlercontext);
  
  cout << "handler context sp: " << handlercontext.uc_mcontext.gregs[REG_RIP] <<
        " ip: " << handlercontext.uc_mcontext.gregs[REG_RSP] <<
        " bp: " << handlercontext.uc_mcontext.gregs[REG_RBP] << endl;
  
  size_t* _ptr;
  _ptr = (size_t*) handlercontext.uc_mcontext.gregs[REG_RBP];
  
  cout << "ptr[0] - current sp" << *(_ptr) << endl; _ptr--;
  cout << "ptr[-1] - flags " << *(_ptr) << endl; _ptr--;
  cout << "ptr[-2] - SP in main" << *(_ptr) << endl; _ptr--;
  cout << "ptr[-3] - IP in main " << *(_ptr) << endl;
  
}


int main(){
  
  signal(SIGALRM, &sigalrm_handler);
  alarm(1);  
  
  
  getcontext(&maincontext);
  
  cout << "main context sp: " << maincontext.uc_mcontext.gregs[REG_RIP] <<
          " ip: " << maincontext.uc_mcontext.gregs[REG_RSP] << endl;
  
  while(i == 0);
  cout << "back to main, i = " << i << endl;
  
  return 0;
}