#include "prod-con.h"
#include "timer.h"

volatile bool quit = false;

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
    gettimeofday(&startTime, NULL);

    pthread_mutex_lock(t->queue->mut);
    if (t->queue->full) {

      if (t->errorFcn != NULL) {
        t->errorFcn(NULL);
      }

    } else {
      //! Print
      // printf("added item\n");
      job.data = &startTime;
      queueAdd(t->queue, job);
    }    

    pthread_mutex_unlock(t->queue->mut);
    pthread_cond_signal(t->queue->notEmpty);

    gettimeofday(&currentTime, NULL);

    t->tDrift[i] = getTimeDifference(startTime, currentTime);
    unsigned long sleep_us = (long)t->period * 1000 - t->tDrift[i];
    if (sleep_us < 0 && sleep_us > t->period * 1000) {
      sleep_us = 0;
    }
    //! Print
    // printf("sleep%d\n", sleep_us);
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

  struct timeval tJobStart;
  struct timeval tJobEnd;

  while (1) {
    pthread_mutex_lock(conArgs->queue->mut);
    if (conArgs->queue->empty) {
      pthread_cond_wait(conArgs->queue->notEmpty, conArgs->queue->mut);

      if (quit) {
        pthread_mutex_unlock(conArgs->queue->mut);
        pthread_exit(NULL);
      }
    }

    queuePop(conArgs->queue, &job);
    //! Print
    printf("popped\n");

    pthread_mutex_unlock(conArgs->queue->mut);
    pthread_cond_signal(conArgs->queue->notFull);

    gettimeofday(&tJobStart, NULL);
    
    if (job.jobFcn != NULL) {
      job.jobFcn(job.data);
    }

    // TODO: Measure time and save it in array in ConsArgs.
    // conArgs->tOut = 
    // TODO: Create mutex in ConArgs to save the time..
  }
  
  pthread_exit(NULL);
}

void consumerSetQuit(bool flag) {
  quit = flag;
}
