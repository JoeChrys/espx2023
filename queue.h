/*
 *	File	: queue.h
 *
 *	Title	: Queue helper file.
 *
 *	Short	: Type and Function declarations of queue.	
 *
 *	Long 	: It does NOT have overflow protection by design.
 *
 *	Author	: Iosif Chrysostomou
 *
 *	Date	: 
 *
 *	Revised	:
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"


/*
 * Type Job
 * Object that is used in the queue.
 * Contains a function pointer and 2 variable pointer.
 */
typedef struct {
  void* (*jobFcn)(void *);
  void* data;
  // void* tJobStart;
} Job;

/*
 * Type Queue
 * Object that is used as a queue.
 * Contains a statically defined array (type of Job) used to store items.
 * head, tail, full, empty internal variable for proper storing.
 * pthread library variables are used to prevent race conditions.
 */
typedef struct {
  Job buf[QUEUE_SIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} Queue;

Queue *queueInit(void);

void queueDelete(Queue *q);

void queueAdd(Queue *q, Job in);

void queuePop(Queue *q, Job *out);

#endif /* QUEUE_H */