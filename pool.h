#ifndef POOL_H
#define POOL_H

#define META_SIZE 32
#define TXT_SIZE  280
#define MSG_SIZE  TXT_SIZE + 4
#define MSG_MAX   2000
#define MEM_SIZE  MSG_SIZE * MSG_MAX + META_SIZE
#define POOL_NAME "/mem_pool"
#define SEM_NAME  "/mem_sem"

typedef struct {
  int  idx;
  char txt[TXT_SIZE];
} Message;

typedef struct {
  int head;
  int tail;
  Message *msgs;
} MessageBuffer;

extern const struct timespec POLL_RATE;
void inform_and_panic(char *source);
MessageBuffer *msgs_init(void *shared_mem);
MessageBuffer *msgs_retrieve(void *shared_mem);

#endif // POOL_H
