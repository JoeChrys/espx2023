/*
 *	File	: timer.c
 *
 *	Title	: Timer source code.
 *
 *	Short	: Function impenentations of a timer object.	
 *
 *	Long 	:
 *
 *	Author	: Iosif Chrysostomou
 *
 *	Date	: 
 *
 *	Revised	:
 */

#include "timer.h"

#include "common.h"
#include "queue.h"
#include "prod-con.h"
#include <math.h>

static void *stopFcn(void *args) {
  Timer *t = (Timer *)args;

  char filenameIn[32] = {};
  sprintf(filenameIn, "exp%d-period%dms-timeIn", t->expNum+1, t->period);
  FILE *fIn = fopen(filenameIn, "w");

  char filenameDrift[32] = {};
  sprintf(filenameDrift, "exp%d-period%dms-timeDrift", t->expNum+1, t->period);
  FILE *fDrift = fopen(filenameDrift, "w");

  char filenameLostJobs[32] = {};
  sprintf(filenameLostJobs, "exp%d-period%dms-lostJobs", t->expNum+1, t->period);
  FILE *fJobsLost = fopen(filenameLostJobs, "w");

  char filenameOverDrift[32] = {};
  sprintf(filenameOverDrift, "exp%d-period%dms-overDrift", t->expNum+1, t->period);
  FILE *fOverDrift = fopen(filenameOverDrift, "w");

  for (int i=0; i<t->tasksToExecute; i++) {
    fprintf(fIn, "%d\n", t->tIn[i]);
    fprintf(fDrift, "%d\n", t->tDrift[i]);
  }

  fprintf(fJobsLost, "%d\n", t->lostJobs);
  fprintf(fOverDrift, "%d\n", t->overDriftCnt);

  fclose(fIn);
  fclose(fDrift);
  fclose(fJobsLost);
  fclose(fOverDrift);

  free(t->tIn);
  free(t->tDrift);
}

static void *timerFcn(void *args){
  struct timeval tPush = *(struct timeval *)args;
  long secs = (double) tPush.tv_sec;
  long usecs = (double) tPush.tv_usec;

  double resFcn[4] = {};

  //Random math work
  for (int i=0; i<200; i++){
    if (i%2) {
      resFcn[1] += cos(usecs);
      resFcn[2] += sin(usecs);
      resFcn[3] += tan(usecs);
    } else {
      resFcn[1] -= cos(secs);
      resFcn[2] -= sin(secs);
      resFcn[3] -= tan(secs);
    }
  }
  resFcn[0] = sqrt(fabs(resFcn[1] + resFcn[2] + resFcn[3]));

  //Random memory storing
  double *heap = (double *)calloc(sizeof(*resFcn), sizeof(resFcn));
  for (int i=0; i<sizeof(resFcn)/sizeof(*resFcn); i++){
    heap[i] = resFcn[i];
  }
  free(heap);

}

static void *errorFcn(void *args){
  *(int *)args = *((int *) args) + 1;
}

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
  t->timerFcn = timerFcn;
  t->errorFcn = errorFcn;

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

  t->lostJobs = 0;
  t->overDriftCnt = 0;
  
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

// Function to calculate the time difference in microseconds
int getTimeDifference(struct timeval start, struct timeval end) {
  return ((end.tv_sec - start.tv_sec) * 1000000) + ((end.tv_usec - start.tv_usec));
}
