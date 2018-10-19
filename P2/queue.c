/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "queue.h"

Queue *CreateStringQueue(int size) {
	Queue *q;
	if((q = calloc(1, sizeof(Queue))) == NULL) {
		fprintf(stderr, "Cannot calloc for Queue\n");
		return NULL;
	}
	if((q->element = calloc(size, sizeof(char*))) == NULL) {
		fprintf(stderr, "Cannot calloc for element\n");
		free(q);
		return NULL;
	}
	q->enqueueCount = 0;
	q->dequeueCount = 0;
	q->enqueueBlockCount = 0;
	q->dequeueBlockCount = 0;
	q->first = 0;
	q->last = 0;
	q->size = size;
	Pthread(cond_init,  &q->q_full,  NULL);
	Pthread(cond_init,  &q->q_empty, NULL);
	Pthread(mutex_init, &q->q_lock,  NULL);
	return q;
}

void DestroyStringQueue(Queue* q) {
	Pthread(mutex_lock, &(q->q_lock));
	for (int i = q->first; i != q->last; i = (i + 1) % q->size)
		if (q->element[i] != NULL)
			free(q->element[i]);
	free(q->element);
	Pthread(cond_destroy,  &q->q_empty);
	Pthread(cond_destroy,  &q->q_full);
	Pthread(mutex_unlock,  &q->q_lock);
	Pthread(mutex_destroy, &q->q_lock);
	free(q);
}

void EnqueueString(Queue *q, char *string) {
	Pthread(mutex_lock, &q->q_lock);
	while((q->last + 1) % q->size == q->first) {
		q->enqueueBlockCount++;
		Pthread(cond_wait, &q->q_full, &q->q_lock);
	}
	(q->element)[q->last] = string;
	q->last = (q->last + 1) % q->size;
	if (string != NULL)
		q->enqueueCount++;
	Pthread(cond_signal,  &q->q_empty);
	Pthread(mutex_unlock, &q->q_lock);
}

char *DequeueString(Queue *q) {
	Pthread(mutex_lock, &q->q_lock);
	while(q->first == q->last) {
		q->dequeueBlockCount++;
		Pthread(cond_wait, &q->q_empty, &q->q_lock);
	}
	char *string = (q->element)[q->first];
	q->first = (q->first + 1) % q->size;
	if (string != NULL)
		q->dequeueCount++;
	Pthread(cond_signal,  &q->q_full);
	Pthread(mutex_unlock, &q->q_lock);
	return string;
}

void PrintQueueStats(Queue *q){
	Pthread(mutex_lock, &q->q_lock);
	fprintf(stderr, "enqueueCount is %d\n", q->enqueueCount);
	fprintf(stderr, "dequeueCount is %d\n", q->dequeueCount);
	fprintf(stderr, "enqueueBlockCount is %d\n", q->enqueueBlockCount);
	fprintf(stderr, "dequeueBlockCount is %d\n", q->dequeueBlockCount);
	Pthread(mutex_unlock, &q->q_lock);
}

