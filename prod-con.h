#ifndef PROD_CON_H
#define PROD_CON_H

#include "queue.h"

typedef struct{
  Queue *queue;

  int expNum;
  long *tOut;

  pthread_mutex_t *mutOut;
  bool *fileUsed;
  pthread_cond_t *notUsed;
} ConArgs;

void *producer(void *args);

void *consumer(void *args);

void consumerSetQuit(bool flag);

#endif /* PROD_CON_H */