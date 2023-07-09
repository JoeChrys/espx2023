#ifndef PROD_CON_H
#define PROD_CON_H

#include "queue.h"

typedef struct{
  int expNum;
  Queue *queue;
  long *tOut;
} ConArgs;

void *producer(void *args);

void *consumer(void *args);

void consumerSetQuit(bool flag);

#endif /* PROD_CON_H */