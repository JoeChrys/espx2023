#include "prod-con.h"
#include "timer.h"

volatile bool quit = false;
volatile int tasksDone = 0;

void *producer(void* args) {
  Timer *t = (Timer *)args;
  Job job = {
    .jobFcn = t->timerFcn
  };

  struct timeval startTime;
  struct timeval currentTime;

  // startFcn() not used, everything is done in timerInit()
  if (t->startFcn != NULL) {
    t->startFcn(NULL);
  }
  
  for (int i=0; i<t->tasksToExecute; i++) {
    struct timeval *tPush = (struct timeval *)malloc(sizeof(struct timeval));
    gettimeofday(&startTime, NULL);

    pthread_mutex_lock(t->queue->mut);
    if (t->queue->full) {
      if (t->errorFcn != NULL) {
        t->errorFcn(&t->lostJobs);
      }
    } else {
      //! Print
      // printf("added item\n");
      gettimeofday(&currentTime, NULL);
      *tPush = currentTime;

      job.data = tPush;
      queueAdd(t->queue, job);

      t->tIn[i] = getTimeDifference(startTime, currentTime);
    }    

    pthread_mutex_unlock(t->queue->mut);
    pthread_cond_signal(t->queue->notEmpty);

    gettimeofday(&currentTime, NULL);

    t->tDrift[i] = getTimeDifference(startTime, currentTime);
    int sleep_us = t->period * 1000 - t->tDrift[i];
    if (sleep_us < 0 && sleep_us > t->period * 1000) {
      sleep_us = 0;
      t->overDriftCnt++;
    }
    //! Print
    printf("PROD: sleep%d\n", sleep_us);
    usleep(sleep_us);
  }

  if (t->stopFcn != NULL) {
    t->stopFcn(t);
  }

  pthread_exit(NULL);
}

void *consumer(void* args) {
  ConArgs *conArgs = (ConArgs *)args;
  Job job;

  struct timeval tPop;

  while (1) {
    pthread_mutex_lock(conArgs->queue->mut);
    while (conArgs->queue->empty) {
      pthread_cond_wait(conArgs->queue->notEmpty, conArgs->queue->mut);

      if (quit) {
        pthread_mutex_unlock(conArgs->queue->mut);
        pthread_exit(NULL);
      }
    }

    queuePop(conArgs->queue, &job);
    //! Print
    // printf("popped\n");

    pthread_mutex_unlock(conArgs->queue->mut);
    pthread_cond_signal(conArgs->queue->notFull);

    gettimeofday(&tPop, NULL);
    struct timeval tPush;
    tPush = *(struct timeval *)job.data;
    
    pthread_mutex_lock(conArgs->mutOut);

    
    conArgs->tOut[tasksDone] = getTimeDifference(tPush, tPop);
    //! Print
    printf ("CON: tOut %d\n", conArgs->tOut[tasksDone]);
    tasksDone += 1;
    
    pthread_mutex_unlock(conArgs->mutOut);

    //! Print
    // printf("writen to tOut\n");

    if (job.jobFcn != NULL) {
      job.jobFcn(job.data);
    }
    free(job.data);

    //! Print
    printf("CON: job %d done\n", tasksDone);
  }
  
  pthread_exit(NULL);
}

void consumerSetQuit(bool flag) {
  quit = flag;
}

int getTasksDone(void) {
  return tasksDone;
}

void resetTasksDone(void){
  tasksDone = 0;
}
