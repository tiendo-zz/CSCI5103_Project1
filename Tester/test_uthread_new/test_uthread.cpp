#include "uthread.h"
#include <iostream>



using namespace std;

void foo(int arg){
  cout << "Hello " << arg << endl; 
}


int main(){  


  ucontext maincontext;
  getcontext(&maincontext);

  cout << "main context ip: " << maincontext.uc_mcontext.gregs[REG_RIP] <<
      " sp: " << maincontext.uc_mcontext.gregs[REG_RSP] << endl;

  
  for(int i = 0; i < 10; i++){
    uthread_create(&foo, i);
  }
  cout << "foo : " << (size_t) foo << endl;
  cout << "back in main" << endl;       
  int i = 0;
  while(1){
    cout << "inside main ... " << ++i << endl;
    usleep(500000);
  }
  
  return 0;
}