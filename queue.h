#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

typedef struct{
  void* (*jobFcn)(void *);
  void* data;
} job_t;

typedef struct {
  job_t buf[QUEUE_SIZE];
  long head, tail;
  int full, empty;
} queue_t;

queue_t *queueInit (void);
void queueDelete (queue_t *q);
void queueAdd (queue_t *q, job_t in);
void queuePop (queue_t *q, job_t *out);

#endif