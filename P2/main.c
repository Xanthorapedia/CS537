#include "queue.h"
#include "worker.h"

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
	fprintf(stderr, "********Queue Stats********\n");
	char *qname[] = { "Reader", "Munch1", "Munch2", "Writer" };
	for (int i = 0; i < 3; i++) {
		fprintf(stderr, "Pipe %d (%s -> %s):\n", i, qname[i], qname[i + 1]);
		PrintQueueStats(q[i + 1]);
		if(i < 2)
			fprintf(stderr, "\n");
		DestroyStringQueue(q[i + 1]);
	}
	fprintf(stderr, "***************************\n");
	return 0;
}

