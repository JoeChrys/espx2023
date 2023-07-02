#include "common.h"
#include "prod_con.h"

void start(timer_t *t);
void startat(timer_t *t, int year, int month, int day, int hour, int minute, int second);



int main(){
  timer_cfg cfg1, cfg2, cfg3;
  timer_t* t1, t2, t3;

  cfg1 = make_cfg(1000, QUEUE_SIZE, 10);

  if (check_cfg(&cfg1)){
    t1 = StartFcn(&cfg1);
    if (t1 == NULL){
      fprintf (stderr, "main: Timer 1 Init failed.\n");
      exit (1);
    }
  }

  //start(timer)
    //while !end_of_experiment
    //  sleep
    //pthread_join(producer)
  //StopFcn(timer)
  //return
}



void start(timer_t* t){
  pthread_t consumerThread;

  TimerFcn (t);
  pthread_create(&consumerThread, NULL, producer, t);
  for (int i=0; i<NUMBER_OF_THREADS; ++i){
    // may need to add & in first arg
    pthread_create(t->consumerThreads[i], NULL, consumer, t);
  }

  int remainingTime;
  do {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  remainingTime = ((int)currentTime.tv_sec+RUNTIME_SECS - (int)currentTime.tv_sec);
  sleep(60);
  } while (remainingTime > 0);
  
  pthread_cancel(consumerThread);
  for (int i=0; i<NUMBER_OF_THREADS; ++i){
    pthread_cancel(t->consumerThreads[i]);
  }

  pthread_join(consumerThread, NULL);
  for (int i=0; i<NUMBER_OF_THREADS; ++i){
    pthread_join(t->consumerThreads[i], NULL);
  }
}

void startat(timer_t* t, int year, int month, int day, int hour, int minute, int second) {
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

  while (startDelay > 0) {
    sleep(1);
  }

  start(t);
}

// Function to calculate the time difference in milliseconds
int getTimeDifference(struct timeval start, struct timeval end) {
  return ((end.tv_sec - start.tv_sec) * 1000) + ((end.tv_usec - start.tv_usec) / 1000);
}