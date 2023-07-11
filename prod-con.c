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
        t->errorFcn(&t->lostJobs);
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
    printf("sleep%d\n", sleep_us);
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

    gettimeofday(&tPop, NULL);
    struct timeval tPush;
    tPush = *(struct timeval *)job.data;
    
    pthread_mutex_lock(conArgs->mutOut);

    //! Prints
    printf ("before %d\n", *(conArgs->tasksDone));
    conArgs->tOut[*(conArgs->tasksDone)] = getTimeDifference(tPush, tPop);
    *(conArgs->tasksDone) += 1;
    printf ("tOut %d\n", conArgs->tOut[*(conArgs->tasksDone)]);
    printf ("after %d\n", *(conArgs->tasksDone));

    pthread_mutex_unlock(conArgs->mutOut);

    //! Print
    printf("writen to tOut\n");

    if (job.jobFcn != NULL) {
      job.jobFcn(job.data);
    }

    //! Print
    printf("job done\n");

    // TODO: Measure time and save it in array in ConsArgs.
    // conArgs->tOut = 
    // TODO: Create mutex in ConArgs to save the time..
  }
  
  pthread_exit(NULL);
}

void consumerSetQuit(bool flag) {
  quit = flag;
}
