#include "timer.h"

#include "common.h"
#include "queue.h"
#include "prod-con.h"

static void *stopFcn(void *args) {
  Timer *t = (Timer *)args;

  char filenameIn[32] = {};
  sprintf(filenameIn, "%d-prod-%d time", t->expNum, t->period);
  FILE *fIn = fopen(filenameIn, "w");

  char filenameDrift[32] = {};
  sprintf(filenameDrift, "%d-prod-%d drift", t->expNum, t->period);
  FILE *fDrift = fopen(filenameDrift, "w");

  for (int i=0; i<t->tasksToExecute; i++) {
    fprintf(fIn, "%d\n", t->tIn[i]);
    fprintf(fDrift, "%d\n", t->tDrift[i]);
  }

  fclose(fIn);
  fclose(fDrift);

  free(t->tIn);
  free(t->tDrift);
}


// TODO: Implement timerFcn().


Timer *timerInit(int period, Queue *queue, int expNum) {
  if (period < 1) {
    fprintf (stderr, "timer: incorrect period\n");
    return NULL;
  }

  Timer *t = (Timer *)malloc(sizeof(Timer));
  if (t == NULL) {
    fprintf (stderr, "timer: init failed.\n");
    return NULL;
  }

  t->period = period;
  t->tasksToExecute = RUNTIME_SECS * 1000 / period;
  t->startDelay = 0;
  t->queue = queue;
  t->expNum = expNum;

  t->startFcn = NULL;
  t->stopFcn = stopFcn;
  t->timerFcn = NULL;
  t->errorFcn = NULL;

  t->tIn = (int *)malloc(t->tasksToExecute * sizeof(int));
  if (t->tIn == NULL) {
    fprintf (stderr, "timer: tIn init failed.\n");
    return NULL;
  }
  t->tDrift = (int *)malloc(t->tasksToExecute * sizeof(int));
  if (t->tDrift == NULL) {
    fprintf (stderr, "timer: tDrift init failed.\n");
    return NULL;
  }
  
  return t;
}

void timerStart(Timer *t){
  pthread_create(&t->proThread, NULL, producer, t);
}

void timerStartat(Timer *t, int year, int month, int day, int hour, int minute, int second) {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);

  struct tm startTm;
  startTm.tm_sec = second;
  startTm.tm_min = minute;
  startTm.tm_hour = hour;
  startTm.tm_mday = day;
  startTm.tm_mon = month - 1;
  startTm.tm_year = year - 1970;
  startTm.tm_isdst = -1;

  time_t startTime = mktime(&startTm);
  int startDelay = (int)(startTime - currentTime.tv_sec);

  sleep(startDelay);

  timerStart(t);
}

// Function to calculate the time difference in milliseconds
int getTimeDifference(struct timeval start, struct timeval end) {
  return ((end.tv_sec - start.tv_sec) * 1000) + ((end.tv_usec - start.tv_usec) / 1000);
}
