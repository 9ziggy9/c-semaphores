#include <fcntl.h>     // For O_* constants
#include <sys/mman.h>  // For shared memory
#include <unistd.h>    // For ftruncate
#include <stdio.h>     // For printf/puts
#include <stdlib.h>    // For EXIT_FAILURE
#include <semaphore.h> // For semaphore operations
#include <time.h>      // nanosleep() timespec
#include <string.h>
#include <signal.h>
#include "pool.h"

#define SZ_ARG_BUFFER 32
static char CLIENT_NAME[SZ_ARG_BUFFER];

void read_client_name(int num_args, char *cli_arg) {
  if (num_args < 2) {
    printf("ERROR: didn't supply client name.");
    exit(EXIT_FAILURE);
  }
  strncpy(CLIENT_NAME, cli_arg, SZ_ARG_BUFFER);
  CLIENT_NAME[SZ_ARG_BUFFER] = '\0';
}

void broadcast_entry(char *m_loc) {
  sem_t *SEM = sem_open(SEM_NAME, 0);
  if (SEM == SEM_FAILED) inform_and_panic("sem_open");

  sem_wait(SEM); // Acquire the semaphore before writing
  sprintf(m_loc, "%s has entered the memory pool.\n", CLIENT_NAME);
  sem_post(SEM); // Release the semaphore
}

int main(int argc, char *argv[]) {
  read_client_name(argc, argv[1]);

  int shm_fd = shm_open(POOL_NAME, O_RDWR, 0666);
  if (shm_fd == -1) inform_and_panic("shm_open");

  char *m_loc = mmap(
    0,
    MEM_SIZE,
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    shm_fd, 0
  );

  if (m_loc == MAP_FAILED) inform_and_panic("mmap");

  broadcast_entry(m_loc);

  while (1) {
    printf("%s", m_loc);
    fflush(stdout);
    nanosleep(&POLL_RATE, NULL);
  }

  return 0;
}
