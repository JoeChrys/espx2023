#ifndef PROD_CON_H
#define PROD_CON_H

#include "timer.h"

typedef struct{
  int jobNumber;
  int threadId;
} userData_t;

void *producer(void *args);
void *consumer(void *args);

#endif