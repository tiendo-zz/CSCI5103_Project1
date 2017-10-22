#include <aio.h>
#include <signal.h>
#include <iostream>
#include <strings.h>
// #include <fstream>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


#define BUF_SIZE 8309

size_t file_finish = 0;
struct aiocb my_aiocb;


void aio_completion_handler( int signo, siginfo_t *info, void *context )
{
  std::cout << "inside handler" << std::endl;
  struct aiocb *req;  
 
  /* Ensure it's our signal */
  if (info->si_signo == SIGIO) {
 
    req = (struct aiocb *)info->si_value.sival_ptr;
 
    /* Did the request complete? */
    if (aio_error( req ) == 0) {
 
      /* Request completed successfully, get the return status */
      file_finish = aio_return( req );
      std::cout << "aio return: " << file_finish << std::endl;
      std::cout << "aio fd: " << req->aio_fildes << std::endl;
    }
  }
 
  return;
}

void setup_io( )
{
  int fd, ret;
  struct sigaction sig_act;
  
  
  fd = open( "/tien/vicon_test/sriswf_params.txt", O_RDONLY   );  
  std::cout << "fd: " << fd << std::endl;
  
  /* Set up the signal handler */
  sigemptyset(&sig_act.sa_mask);
  sig_act.sa_flags = SA_SIGINFO;
  sig_act.sa_sigaction = aio_completion_handler;
 
 
  /* Set up the AIO request */
  bzero( (char *)&my_aiocb, sizeof(struct aiocb) );
  my_aiocb.aio_fildes = fd; //STDIN_FILENO;
  my_aiocb.aio_buf = new char[BUF_SIZE+1];
  my_aiocb.aio_nbytes = BUF_SIZE;
  my_aiocb.aio_offset = 0;
 
  /* Link the AIO request with the Signal Handler */
  my_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  my_aiocb.aio_sigevent.sigev_signo = SIGIO;
  my_aiocb.aio_sigevent.sigev_value.sival_ptr = &my_aiocb;
 
  /* Map the Signal to the Signal Handler */
  ret = sigaction( SIGIO, &sig_act, NULL );  
  ret = aio_read( &my_aiocb );  
}


int main(){
 
  setup_io();
  while(!file_finish);    
    
  char* val; val = (char*) file_finish;
  // std::cout << "file: " << val[0] << std::endl;
//   printf("%s", my_aiocb.aio_buf);
  
  while(1);
  return 0;
}