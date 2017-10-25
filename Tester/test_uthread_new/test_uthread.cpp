#include "uthread.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



using namespace std;

void *foo(void *arg){
  cout << "Hello from " << uthread_self() << endl; 
}




int main(){  
  ucontext maincontext;
  getcontext(&maincontext);

  uthread_init(100000);

  cout << "main context ip: " << maincontext.uc_mcontext.gregs[REG_RIP] <<
      " sp: " << maincontext.uc_mcontext.gregs[REG_RSP] << endl;

  
  for(int i = 0; i < 3; i++){
    uthread_create(&foo, &i);
  }
  cout << "foo : " << (size_t) foo << endl;
  cout << "back in main" << endl;       
  int i = 0;


  for (int i = 1; i < 4; i++) {
    void **retval;
    cout << "joining thread " << i << " is " << uthread_join(i, retval) << endl;
  }

  cout << "10 threads joined\n";

  for (int i = 0; i < 3; i++)
    uthread_create(&foo, &i);

  while (1);
  
  return 0;
}
