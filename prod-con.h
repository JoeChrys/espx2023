#ifndef PROD_CON_H
#define PROD_CON_H

#include "queue.h"

typedef struct{
  Queue *queue;

  int expNum;
  int *tOut;
  pthread_mutex_t *mutOut;

} ConArgs;

void *producer(void *args);

void *consumer(void *args);

void consumerSetQuit(bool flag);

int getTasksDone(void);
void resetTasksDone(void);

#endif /* PROD_CON_H */