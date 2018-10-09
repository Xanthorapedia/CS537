#include "queue.h"

void Pthread_mutex_lock(pthread_mutex_t *mutex);
void Pthread_mutex_unlock(pthread_mutex_t *mutex);
void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *q_lock);
void Pthread_cond_signal(pthread_cond_t *cond);
void Pthread_mutex_destroy(pthread_mutex_t *mutex);

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
    assert(pthread_mutex_init(&q->q_lock, NULL));
    assert(pthread_cond_init(&q->q_full, NULL));
    assert(pthread_cond_init(&q->q_empty, NULL));
	return q;
}

void EnqueueString(Queue *q, char *string) {
	Pthread_mutex_lock(&(q->q_lock));
	while((q->last + 1) % q->size == q->first) {
		q->enqueueBlockCount++;
		Pthread_cond_wait(&(q->q_full), &(q->q_lock));
	}
	(q->element)[q->last] = string;
	q->last = (q->last + 1) % q->size;
	q->enqueueCount++;
	Pthread_cond_signal(&(q->q_empty));
	Pthread_mutex_unlock(&(q->q_lock));
}

char *DequeueString(Queue *q) {
	char *str;
	Pthread_mutex_lock(&(q->q_lock));
	while(q->first == q->last) {
		q->enqueueBlockCount++;
		Pthread_cond_wait(&(q->q_empty), &(q->q_lock));
	}
	str = (q->element)[q->first];
	q->first = (q->first + 1) % q->size;
	q->dequeueCount++;
	Pthread_cond_signal(&(q->q_full));
	Pthread_mutex_unlock(&(q->q_lock));
	return str;
}

void PrintQueueStats(Queue *q){
	Pthread_mutex_lock(&(q->q_lock));
	fprintf(stderr, "enqueueCount is %d\n", q->enqueueCount);
	fprintf(stderr, "dequeueCount is %d\n", q->dequeueCount);
	fprintf(stderr, "enqueueBlockCount is %d\n", q->enqueueBlockCount);
	fprintf(stderr, "dequeueBlockCount is %d\n", q->dequeueBlockCount);
	Pthread_mutex_unlock(&(q->q_lock));
}

void Pthread_mutex_lock(pthread_mutex_t *mutex) {
	assert(pthread_mutex_lock(mutex) == 0);
}

void Pthread_mutex_unlock(pthread_mutex_t *mutex) {
	assert(pthread_mutex_unlock(mutex) == 0);
}

void Pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *q_lock) {
	assert(pthread_cond_wait(cond, q_lock) == 0);
}

void Pthread_cond_signal(pthread_cond_t *cond) {
	assert(pthread_cond_signal(cond) == 0);
}

void Pthread_mutex_destroy(pthread_mutex_t *mutex) {
	assert(pthread_mutex_destroy(mutex) == 0);
}
