#include "timer.h"

int check_cfg(timer_cfg* cfg){
  if (cfg->period < 1){
    printf("Incorrect Period Value");
    return 0;
  }
  if (cfg->tasksToExecute < 0){
    printf("Incorrect tasksToExecute Value");
    return 0;
  }
  if (cfg->startDelay < 0){
    printf("Incorrect startDelay Value");
    return 0;
  }
  return 1;
}

timer_cfg make_cfg(int period, int tasksToExecute, int startDelay){
  timer_cfg cfg;
  cfg.period = period;
  cfg.tasksToExecute = tasksToExecute;
  cfg.startDelay = startDelay;
  return cfg;
}

timer_t *StartFcn(timer_cfg* cfg){
  timer_t *t;

  t = (timer_t *) malloc(sizeof (timer_t));
  if (t == NULL) {
    return (NULL);
    printf("Error allocating memory for timer object");
  }


  //remember to also free at delete
  t->queue = queueInit();
  
  t->period = cfg->period;
  t->tasksToExecute = cfg->tasksToExecute;
  t->startDelay = cfg->startDelay;

  t->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (t->mut, NULL);
  t->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (t->notFull, NULL);
  t->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (t->notEmpty, NULL);

  for (int i=0; i<NUMBER_OF_THREADS; ++i){
    t->consumerThreads[i] = (pthread_t *) malloc(sizeof(pthread_t));
    if (t->consumerThreads[i] == NULL) {
      return NULL;
      printf("Error allocating memory for consumer threads");
    }
  }
}

void StopFcn(timer_t *t){
  for (int i=0; i<NUMBER_OF_THREADS; ++i){
    pthread_join(t->consumerThreads[i], NULL);
  }

  free(t->consumerThreads);

  pthread_mutex_destroy (t->mut);
  free (t->mut);	
  pthread_cond_destroy (t->notFull);
  free (t->notFull);
  pthread_cond_destroy (t->notEmpty);
  free (t->notEmpty);
  free (t->queue);

  free(t);
}