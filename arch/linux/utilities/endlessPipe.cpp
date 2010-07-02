#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME argv[1]

int main(int argc, char ** argv)
{
  if(argc < 2){
    perror("arg count");
    return EXIT_FAILURE;
  }

  umask(0);

  if ( mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST ){
    perror("mkfifo()");
    return EXIT_FAILURE;
  }

  int fd = open(FIFO_NAME, O_RDONLY);

  if (fd == -1){
    perror("open()");
    return EXIT_FAILURE;
  }

  int error = 0;

  char ch;

  while (error == 0) {
    int readRet = read(fd, &ch, 1);

    if (readRet == -1) {
      perror("read()");
      error = 1;

    /* there is no more reader; close the FIFO and reopen it to wait for */
    /* a new one */
    }else if (readRet == 0) {
      close(fd);
      fd = open(FIFO_NAME, O_RDONLY);
    }else{
      putchar(ch);
      fflush(stdout);
    }
  }

  close(fd);

  return EXIT_FAILURE;
}

