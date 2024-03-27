#include "pool.h"
#include <time.h>     // nanosleep() timespec
#include <stdio.h>    // perror()
#include <stdlib.h>   // EXIT
#include <string.h>   // memcpy
#include <sys/mman.h> // mmap

const struct timespec POLL_RATE = {
  .tv_sec  = 0,
  .tv_nsec = 16666667, // 1/60 sec
};

void inform_and_panic(char *source) {
  perror(source);
  exit(EXIT_FAILURE);
}

void panic_on_bad_share(void *shared_mem) {
  if (shared_mem == MAP_FAILED) {
    fprintf(stderr, "msgs_init(): passed bad shared_mem");
    exit(EXIT_FAILURE);
  }
}

MessageBuffer *msgs_init(void *shared_mem) {
  panic_on_bad_share(shared_mem);

  // create buffer after meta data
  MessageBuffer *msg_bf = (MessageBuffer *)((char *) shared_mem + META_SIZE);

  msg_bf->head = 0;
  msg_bf->tail = 0;

  // msgs should point to the start of the message array
  // after the MessageBuffer struct
  msg_bf->msgs = (Message *) ((char *) msg_bf + sizeof(MessageBuffer));

  // initialize each message's idx to -1 to show it's empty
  // while not strictly necessary, it helps with debugging
  for (int i = 0; i < MSG_MAX; i++) {
    msg_bf->msgs[i].idx = -1;
    memset(msg_bf->msgs[i].txt, 0, TXT_SIZE); // clean content
  }

  return msg_bf;
}

MessageBuffer *msgs_retrieve(void *shared_mem) {
  panic_on_bad_share(shared_mem);
  return (MessageBuffer *) ((char *) shared_mem + META_SIZE);
}

int msgs_write(MessageBuffer *msg_bf, Message msg) {
  if ((msg_bf->tail + 1) % MSG_MAX == msg_bf->head) {
    // overwrite in the case of full ring buffer
    msg_bf->msgs[msg_bf->head] = msg;
    msg_bf->head = (msg_bf->head + 1) % MSG_MAX;
  } else {
    msg_bf->msgs[msg_bf->tail] = msg;
    msg_bf->tail = (msg_bf->tail + 1) % MSG_MAX;
  }
  return 0;
}

Message msgs_read_latest(MessageBuffer *msg_bf) {
    // Check if buffer is empty
    if (msg_bf->head == msg_bf->tail) {
        Message empty_msg = {.idx = -2};
        return empty_msg;
    }
    
    // The latest message is at the index before the tail
    int latest_idx = (msg_bf->tail - 1 + MSG_MAX) % MSG_MAX; 
    return msg_bf->msgs[latest_idx];
}

int msgs_dump(MessageBuffer *msg_bf) {
    // Generate the filename with timestamp
    time_t epoch_time = time(NULL);
    char filename[30];
    sprintf(filename, "msgs-%ld.dump", epoch_time);

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    int idx = msg_bf->head; 
    while (idx != msg_bf->tail) {
        fprintf(
          fp,
          "Index: %d\nMessage: %s\n\n",
          msg_bf->msgs[idx].idx,
          msg_bf->msgs[idx].txt
        );
        idx = (idx + 1) % MSG_MAX;
    }

    fclose(fp);
    return 0;
}
