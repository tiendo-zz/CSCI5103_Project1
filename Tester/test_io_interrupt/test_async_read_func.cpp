#include "uthread.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

#define BUF_SIZE 100


void PrintHello(int arg){
  cout << "Hello " << arg << endl; 
}

void PrintGoodBye(int arg){
  char filename[50];
  sprintf(filename, "../../../Tester/test_io_interrupt/data/goodbye%d.txt", arg);  
  cout << filename << endl;
  int fd = open(filename, O_RDONLY);
  
  char* buff = new char[BUF_SIZE];
  int ret = async_read(fd, (void*)buff, BUF_SIZE);
  
  // read file sucessfully
  if(ret){
    cout.write(buff, ret);
    cout << endl;
  }
  else
    cout << "can't read this file" << endl;  
}


int main(){      
  uthread_create(&PrintGoodBye, 2);
  uthread_create(&PrintHello, 100);
  while(1);
  return 0;
}
