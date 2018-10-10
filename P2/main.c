#include <pthread.h>
#include "queue.h"
#include "worker.h"

#define Pthread(what,...) assert(pthread_##what(__VA_ARGS__) == 0)
// TODO pthread_create with attr?
// TODO pthread_join retval?
// TODO blockcount -1

int main(int argc, char *argv[]) {
	(void)(argc), (void)(argv);
	// initialize queues (Queue0 is the dummy input for reade)
	Queue *q[4];
	for (int i = 0; i < 3; i++) {
		q[i + 1] = CreateStringQueue(10);
		if (q[i + 1] == NULL) {
			for (i = i -1; i >= 0; i--)
				DestroyStringQueue(q[i + 1]);
			fprintf(stderr, "Queue malloc failed, exiting");
			exit(-1);
		}
	}

	// run thread and wait for completion
	pthread_t worker[4];
	void *(*worker_run[])(void *) = { reader_run, munch1_run, munch2_run, writer_run };
	for (int i = 0; i < 4; i++)
		Pthread(create, &worker[i], NULL, worker_run[i], &q[i]);
	for (int i = 0; i < 4; i++)
		Pthread(join, worker[i], NULL);

	// print stat to stderr
	for (int i = 0; i < 3; i++) {
		PrintQueueStats(q[i + 1]);
		DestroyStringQueue(q[i + 1]);
	}
	return 0;
}
