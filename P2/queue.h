#include <ctype.h>
#include <stdio.h>

typedef struct {
	// TODO add more necessary parts

	// stats
	int enqueueCount;
	int dequeueCount;
	int enqueueBlockCount;
	int dequeueBlockCount;
} Queue;

// Dynamically allocate a new Queue structure and initialize it with an array
// of character points of length size. Returns a pointer to the new queue
// structure.
Queue *CreateStringQueue(int size);

// Places the pointer to the string at the end of queue q. If the queue is full,
// then this function blocks until there is space available.
void EnqueueString(Queue *q, char *string);

// Removes a pointer to a string from the beginning of queue q. If the queue is
// empty, then this function blocks until there is a string placed into the
// queue. Returns the pointer that was removed from the queue.
char * DequeueString(Queue *q);

// Prints the statistics for this queue to stderr (Details 8).
void PrintQueueStats(Queue *q);

