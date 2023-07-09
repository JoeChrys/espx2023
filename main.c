#include "queue.h"
#include "prod-con.h"
#include "timer.h"


int main() {

  int periods[] = {1000, 100, 10};
  long *tOut;
  pthread_mutex_t *mutOut;

  // EXP 1-2-3-...
  for (int i=0; i<sizeof(periods)/sizeof(*periods); i++) {
    //! Print
    printf("started experiment %d\n", i);
    Queue *queue = queueInit();

    Timer *t = timerInit(periods[i], queue, i);

    tOut = (long *)malloc(t->tasksToExecute * sizeof(long));
    
    
    ConArgs args = {
      .expNum = i,
      .queue = queue,
      .tOut = tOut
    };

    // Creates consumers.
    pthread_t conThreads[NUMBER_OF_THREADS];
    for (int j=0; j<NUMBER_OF_THREADS; j++) {
      pthread_create(&conThreads[j], NULL, consumer, &args);
    }

    timerStart(t);

    // Waits for producers to end.
    pthread_join(t->proThread, NULL);

    // Waits for consumers to end.
    consumerSetQuit(true);
    pthread_cond_broadcast(queue->notEmpty);
    for (int j=0; j<NUMBER_OF_THREADS; j++){
      pthread_join(conThreads[j], NULL);
    }
    consumerSetQuit(false);

    free(tOut);
    free(t);

    queueDelete(queue);
  }

  //! Print
  printf("started experiment 4\n");
  
  Queue *queue = queueInit();

  Timer *t[3];
  int memsize = 0;
  for (int i=0; i<3; i++) {
    t[i] = timerInit(periods[i], queue, i);
    memsize += t[i]->tasksToExecute * sizeof(long);
  }
  tOut = (long *)malloc(memsize);

  ConArgs args = {
    .expNum = 4,
    .queue = queue,
    .tOut = tOut
  };

  // Creates consumers.
  pthread_t conThreads[NUMBER_OF_THREADS];
  for (int j=0; j<NUMBER_OF_THREADS; j++) {
    pthread_create(&conThreads[j], NULL, consumer, &args);
  }

  for (int i=0; i<3; i++){
    timerStart(t[i]);
  }

  // Waits for producers to end.
  for (int i=0; i<3; i++){
    pthread_join(t[i]->proThread, NULL);
  }

  // Waits for consumers to end.
  consumerSetQuit(true);
  pthread_cond_broadcast(queue->notEmpty);
  for (int j=0; j<NUMBER_OF_THREADS; j++){
    pthread_join(conThreads[j], NULL);
  }
  consumerSetQuit(false);

  for (int i=0; i<3; i++){
    free(t[i]);
  }
  free(tOut);
  
  queueDelete(queue);
  return 0;
}

