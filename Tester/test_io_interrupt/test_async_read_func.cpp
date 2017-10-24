#include "uthread.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

#define BUF_SIZE 100


void* PrintHello(void* arg){
  cout << "Hello " << *((int*)arg) << endl; 
}

void* PrintGoodBye(void* arg){
  char filename[50];
  sprintf(filename, "../../../Tester/test_io_interrupt/data/goodbye%d.txt", *((int*)arg));  
  cout << filename << endl;
  int fd = open(filename, O_RDONLY);
  
  if(fd == -1){
    std::cout << "File not exists" << std::endl;
    return NULL;
  }
  
  char* buff = new char[BUF_SIZE];
  int ret = async_read(fd, (void*)buff, BUF_SIZE);
  
  // read file sucessfully
  if(ret > 0){
    cout.write(buff, ret);
    cout << endl;
  }
  else
    cout << "can't read this file" << endl;  
}


int main(){      
  int i = 2;
  uthread_create(&PrintGoodBye, &i);
  int j = 100;
  uthread_create(&PrintHello, &j);
  
  
  while(1);
  return 0;
}
