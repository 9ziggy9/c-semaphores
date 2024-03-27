#include <fcntl.h>    // For O_* constants
#include <sys/mman.h> // For shared memory
#include <unistd.h>   // For ftruncate
#include <stdio.h>    // For printf/puts
#include <stdlib.h>   // For EXIT_FAILURE
#include <semaphore.h>
#include <string.h>
#include <signal.h>

// Dislike globals but won't worry for now...
static void *M_LOC = NULL;
static int SHM_FD = -1;
static const size_t MEM_SIZE = 4096;
static const char *POOL_NAME = "/mem_pool";
static const char *SEM_NAME = "/mem_sem"; 
static sem_t *SEM = NULL; // Pointer to the semaphore

void handle_sigint(int sig) {
  printf("\nReceived signal: %d. Unmapping shared memory and cleaning...", sig);
  if (M_LOC != NULL) {
    munmap(M_LOC, MEM_SIZE);
    M_LOC = NULL;
  }
  if (SHM_FD != -1) {
    close(SHM_FD);
    shm_unlink(POOL_NAME);
    SHM_FD = -1;
  }
  if (SEM != NULL) {
    sem_destroy(SEM); // Release the semaphore
    SEM = NULL;
  }
  exit(EXIT_SUCCESS);
}

void inform_and_panic(char *source) {
  perror(source);
  exit(EXIT_FAILURE);
}

void broadcast_ran_num(char *origin) {
  while (1) {
    sem_wait(SEM); // Acquire the semaphore
    // Critical section - only one process can write to M_LOC at a time
    sprintf(M_LOC, "%s: %d\n", origin, (rand() % 10) + 1);
    sem_post(SEM); // Release the semaphore
    sleep(1); 
  }
}

#define SZ_ARG_BUFFER 32
int main(int argc, char **argv) {
  signal(SIGINT, handle_sigint);

  if (argc < 2) {
    printf("Please provide a name for the pool host.\n");
    exit(EXIT_FAILURE);
  }

  char originator[SZ_ARG_BUFFER];
  strncpy(originator, argv[1], SZ_ARG_BUFFER);
  originator[SZ_ARG_BUFFER] = '\0';

  SHM_FD = shm_open(POOL_NAME, O_CREAT | O_RDWR, 0666);
  if (SHM_FD == -1) inform_and_panic("shm_open");

  if (ftruncate(SHM_FD, MEM_SIZE) == -1) inform_and_panic("ftruncate"); 

  M_LOC = mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, SHM_FD, 0);
  if (M_LOC == MAP_FAILED) inform_and_panic("mmap");

  // Initialize the semaphore with an initial value of 1
  SEM = sem_open(SEM_NAME, O_CREAT, 0666, 1);
  if (SEM == SEM_FAILED) inform_and_panic("sem_open");

  broadcast_ran_num(originator);

  // Clean up (unreachable, but included for good practice)
  if (SEM != NULL) sem_destroy(SEM); 
  exit(EXIT_SUCCESS); 
}
