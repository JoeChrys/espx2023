#include "queue.h"

Queue *queueInit(void) {
  Queue *q = (Queue *)malloc(sizeof(Queue));
  if (q == NULL) {
    fprintf(stderr, "queue: queue init failed");
    return NULL;
  }

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;

  q->mut = (pthread_mutex_t *)malloc(sizeof (pthread_mutex_t));
  if (q->mut == NULL) {
    fprintf(stderr, "queue: mutex key init failed");
    return NULL;
  }
  pthread_mutex_init(q->mut, NULL);

  q->notFull = (pthread_cond_t *)malloc(sizeof (pthread_cond_t));
  if (q == NULL) {
    fprintf(stderr, "queue: notFull init failed");
    return NULL;
  }
  pthread_cond_init (q->notFull, NULL);

  q->notEmpty = (pthread_cond_t *)malloc(sizeof (pthread_cond_t));
  if (q == NULL) {
    fprintf(stderr, "queue: notEmpty init failed");
    return NULL;
  }
  pthread_cond_init (q->notEmpty, NULL);
	
  return q;
}

void queueDelete(Queue *q) {
  pthread_mutex_destroy(q->mut);
  free(q->mut);	
  pthread_cond_destroy(q->notFull);
  free(q->notFull);
  pthread_cond_destroy(q->notEmpty);
  free(q->notEmpty);

  free(q);
}

void queueAdd(Queue *q, Job in) {
  q->buf[q->tail] = in;
  q->tail++;
  if (q->tail == QUEUE_SIZE) {
    q->tail = 0;
  }
  if (q->tail == q->head) {
    q->full = 1;
  }
  q->empty = 0;

  return;
}

void queuePop(Queue *q, Job *out) {
  *out = q->buf[q->head];

  q->head++;
  if (q->head == QUEUE_SIZE) {
    q->head = 0;
  }
  if (q->head == q->tail) {
    q->empty = 1;
  }
  q->full = 0;

  return;
}
