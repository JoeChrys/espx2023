#include "queue.h"
#include "prod-con.h"
#include "timer.h"


int main() {

  int periods[] = {1000, 100, 10};

  for (int i=0; i<sizeof(periods)/sizeof(*periods); i++) {
    printf("started experiment %d\n", i);
    Queue *queue = queueInit();

    Timer *t = timerInit(periods[i], queue, i);

    int *tOut = (int *)malloc(t->tasksToExecute * sizeof(int));
    
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

  return 0;
}

