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
  cout << "Hello " << *((int*)arg) << endl; 
}




int main(){  
  ucontext maincontext;
  getcontext(&maincontext);


  // change time slice to 100ms
  uthread_init(100000);


  cout << "main context ip: " << maincontext.uc_mcontext.gregs[REG_RIP] <<
      " sp: " << maincontext.uc_mcontext.gregs[REG_RSP] << endl;

  
  for(int i = 0; i < 10; i++){
    uthread_create(&foo, &i);
  }
  cout << "foo : " << (size_t) foo << endl;
  cout << "back in main" << endl;       
  int i = 0;


  while(1){
    cout << "inside main ... " << ++i << endl;

    void **retval;

    uthread_join(10, retval);

    if (i == 5)
	uthread_suspend(5);
    if (i == 10) {
        uthread_resume(5);
    }

    if (i == 15)
      break;

    usleep(500000);
  }

  
  return 0;
}
