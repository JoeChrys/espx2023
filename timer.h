#ifndef TIMER_H
#define TIMER_H

#include "common.h"
#include "queue.h"

typedef struct {
  int period;
  int tasksToExecute;
  int startDelay;
} timer_cfg;

typedef struct{
  // period in ms
  int period;
  int tasksToExecute;
  // startDelay in s
  int startDelay;
  pthread_t *consumerThreads[NUMBER_OF_THREADS];
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
  queue_t *queue;
} timer_t;

int check_cfg(timer_cfg* cfg);
timer_cfg make_cfg(int period, int tasksToExecute, int startDelay);

timer_t *StartFcn(timer_cfg* cfg);
void StopFcn(timer_t* t);
void TimerFcn(timer_t* t);
void ErrorFcn(timer_t* t);

#endif
