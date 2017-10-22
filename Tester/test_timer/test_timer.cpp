#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <ucontext.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <aio.h>


using namespace std;

#define BUFSIZE 100

// global vars
int i = 0;
ucontext_t maincontext, handlercontext;
struct aiocb my_aiocb;


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
  cout << "ptr[-2] - IP in main " << *(_ptr) << endl; _ptr--;
  cout << "ptr[-3] - SP in main " << *(_ptr) << endl;
  
}


void aio_handler( int signo, siginfo_t *info, void *context ) {
  cout << "i/o complete\n";

  cout << "sig info fd = " << info->si_fd << endl;
  cout << "buf = " << (char*)(my_aiocb.aio_buf) << endl;
  ++i;
}


int main(){
  
//  signal(SIGALRM, &sigalrm_handler);
//  alarm(1); 

/*
  struct sigaction sa;
  struct itimerval timer;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &sigalrm_handler;
  sigaction(SIGVTALRM, &sa, NULL);

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 10000; // 10ms = 10000 us
  timer.it_interval = timer.it_value;

  setitimer(ITIMER_VIRTUAL, &timer, NULL); 
  std::cout << "config timer interrupt\n";
*/

  
  int fd;


  fd = open("./input.txt", O_RDONLY);

  int fd2 = open("./input2.txt", O_RDONLY);

  bzero((char*)&my_aiocb, sizeof(struct aiocb));

  /* Allocate a data buffer for the aiocb request */
  my_aiocb.aio_buf = (char*)malloc(BUFSIZE+1);
 
  /* Initialize the necessary fields in the aiocb */
  my_aiocb.aio_fildes = fd;
  my_aiocb.aio_nbytes = BUFSIZE;
  my_aiocb.aio_offset = 0;
  
  my_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  my_aiocb.aio_sigevent.sigev_signo = SIGIO;
  my_aiocb.aio_sigevent.sigev_value.sival_ptr = &my_aiocb;

  struct sigaction sig_act;

  sigemptyset(&sig_act.sa_mask);
  sig_act.sa_flags = SA_SIGINFO;
  sig_act.sa_sigaction = aio_handler;
 
  /* Map the Signal to the Signal Handler */
  sigaction( SIGIO, &sig_act, NULL );
 

  cout << "fd = " << fd << endl;
  cout << "fd2 = " << fd2 << endl;

  aio_read(&my_aiocb);
  
  
  getcontext(&maincontext);
  
  cout << "main context ip: " << maincontext.uc_mcontext.gregs[REG_RIP] <<
          " sp: " << maincontext.uc_mcontext.gregs[REG_RSP] << endl;
  
  while(i == 0);

  cout << "back to main, i = " << i << endl;
  
  return 0;
}
