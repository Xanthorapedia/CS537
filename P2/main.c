#include "stdinclude.h"
#include "queue.h"
#include "worker.h"

const int QUEUESIZE = 10;
const int QUEUECOUNT = 3;
const int THREADCOUNT = 4;

// thread runnables and their names
void *(*const worker_run[])(void *) =
	{ reader_run, munch1_run, munch2_run, writer_run };
const char *threadname[] =
	{ "Reader", "Munch1", "Munch2", "Writer" };

int main(int argc, char *argv[]) {
	(void)(argc), (void)(argv);
	// initialize queues (q[0] is the dummy input for reader)
	Queue *q[QUEUECOUNT + 1];
	for (int i = 0; i < QUEUECOUNT; i++) {
		q[i + 1] = CreateStringQueue(QUEUESIZE);
		if (q[i + 1] == NULL) {
			for (i = i -1; i >= 0; i--)
				DestroyStringQueue(q[i + 1]);
			fprintf(stderr, "Queue malloc failed, exiting");
			exit(-1);
		}
	}

	// run thread and wait for completion
	pthread_t worker[THREADCOUNT];
	for (int i = 0; i < THREADCOUNT; i++)
		Pthread(create, &worker[i], NULL, worker_run[i], &q[i]);
	for (int i = 0; i < THREADCOUNT; i++)
		Pthread(join, worker[i], NULL);

	// print stat to stderr
	fprintf(stderr, "********Queue Stats********\n");
	for (int i = 0; i < QUEUECOUNT; i++) {
		fprintf(stderr, "Pipe %d (%s -> %s):\n", i, threadname[i], threadname[i + 1]);
		PrintQueueStats(q[i + 1]);
		if(i < QUEUECOUNT - 1)
			fprintf(stderr, "\n");
		DestroyStringQueue(q[i + 1]);
	}
	fprintf(stderr, "***************************\n");
	return 0;
}

