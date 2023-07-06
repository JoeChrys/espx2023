#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

typedef struct {
  void* (*jobFcn)(void *);
  void* data;
} Job;

typedef struct {
  Job buf[QUEUE_SIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} Queue;

Queue *queueInit(void);

void queueDelete(Queue *q);

void queueAdd(Queue *q, Job in);

void queuePop(Queue *q, Job *out);

#endif /* QUEUE_H */