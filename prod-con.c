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

  if (t->startFcn != NULL) {
    t->startFcn(NULL);
  }

  for (int i=0; i<t->tasksToExecute; i++) {
    gettimeofday(&startTime, NULL);

    usleep(5000);

    pthread_mutex_lock(t->queue->mut);
    if (t->queue->full) {

      if (t->errorFcn != NULL) {
        t->errorFcn(NULL);
      }

    } else {
      printf("added item\n");
      // TODO: Fill the job arguments (if any).
      queueAdd(t->queue, job);
    }    

    pthread_mutex_unlock(t->queue->mut);
    pthread_cond_signal(t->queue->notEmpty);

    gettimeofday(&currentTime, NULL);

    t->tDrift[i] = getTimeDifference(startTime, currentTime);
    int sleep_ms = t->period - t->tDrift[i];
    if (sleep_ms < 0 && sleep_ms > t->period) {
      sleep_ms = 0;
    }
    printf("sleep%d\n", sleep_ms);
    usleep(sleep_ms * 1000);
  }

  if (t->stopFcn != NULL) {
    t->stopFcn(t);
  }

  pthread_exit(NULL);
}

void *consumer(void* args) {
  ConArgs *conArgs = (ConArgs *)args;
  Job job;

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
    printf("popped\n");

    pthread_mutex_unlock(conArgs->queue->mut);
    pthread_cond_signal(conArgs->queue->notFull);

    // start timestamp
    
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
