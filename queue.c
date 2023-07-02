#include "queue.h"


queue_t *queueInit (void)
{
  queue_t *q;

  q = (queue_t *)malloc (sizeof (queue_t));
  if (q == NULL) return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
	
  return (q);
}

void queueDelete (queue_t *q)
{
  free (q);
}

void queueAdd (queue_t *q, job_t in)
{
  q->buf[q->tail] = in;
  q->tail++;
  if (q->tail == QUEUE_SIZE)
  q->tail = 0;
  if (q->tail == q->head)
  q->full = 1;
  q->empty = 0;

  return;
}

void queuePop (queue_t *q, job_t *out)
{
  *out = q->buf[q->head];

  q->head++;
  if (q->head == QUEUE_SIZE)
  q->head = 0;
  if (q->head == q->tail)
  q->empty = 1;
  q->full = 0;

  return;
}
