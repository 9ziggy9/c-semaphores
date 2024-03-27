#include "pool.h"
#include <time.h>   // nanosleep() timespec
#include <stdio.h>  // perror()
#include <stdlib.h> // EXIT

const struct timespec POLL_RATE = {
  .tv_sec  = 0,
  .tv_nsec = 16666667, // 1/60 sec
};

void inform_and_panic(char *source) {
  perror(source);
  exit(EXIT_FAILURE);
}
