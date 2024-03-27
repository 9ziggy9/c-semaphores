#ifndef POOL_H
#define POOL_H

#define MEM_SIZE  4096
#define POOL_NAME "/mem_pool"
#define SEM_NAME  "/mem_sem"


extern const struct timespec POLL_RATE;
void inform_and_panic(char *source);

#endif // POOL_H
