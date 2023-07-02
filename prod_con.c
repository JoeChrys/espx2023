#include "prod_con.h"

void* producer(void* args){
  timer_t *t = (timer_t *) args;
  job_t job;

  struct timeval startTime;
  struct timeval currentTime;

  while (1){
    pthread_testcancel();

    gettimeofday(&startTime, NULL);
    for (int i=0; i<QUEUE_SIZE; ++i) {
      pthread_mutex_lock(t->mut);
      if (t->queue->full){
        ErrorFcn(t);
        break;
      }

      //make userdata
      queueAdd(t, job);

      pthread_mutex_unlock(t->mut);
      pthread_cond_signal(t->notEmpty);

    }
    gettimeofday(&currentTime, NULL);
    usleep (t->period - getTimeDifference(startTime, currentTime));
  }
}

void* consumer(void* args){
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

  timer_t *t = (timer_t *) args;
  job_t job;

  while(1){
    pthread_mutex_lock(t->mut);
    if (t->queue->empty){
      pthread_cond_wait(t->notEmpty, t->mut);
    }
    queuePop(t, &job);

    pthread_mutex_unlock(t->mut);
    pthread_cond_signal(t->notFull);


    // execute TimerFcn with userdata

    pthread_testcancel();
  }
  
  pthread_exit(NULL);
}