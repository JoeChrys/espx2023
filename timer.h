#ifndef TIMER_H
#define TIMER_H

#include "common.h"
#include "queue.h"

typedef struct{
  int period;           //!< Period in ms.
  int tasksToExecute;   //!< Number of tasks to execute.
  int startDelay;
  
  pthread_t proThread;
  Queue *queue;

  void *(*startFcn)(void *args);
  void *(*stopFcn)(void *args);
  void *(*timerFcn)(void *args);
  void *(*errorFcn)(void *args);
  void *userData;

  int *tIn;
  int *tDrift;
  int lostJobs;

  int expNum;
} Timer;

Timer *timerInit(int period, Queue *queue, int expNum);

void timerStart(Timer *t);

void timerStartat(Timer *t, int year, int month, int day, int hour, int minute, int second);

int getTimeDifference(struct timeval start, struct timeval end);

#endif /* TIMER_H */
