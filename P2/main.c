#include <pthread.h>
#include "queue.h"
#include "worker.h"

// TODO pthread_create with attr?
// TODO pthread_join retval?

int main(int argc, char *argv[]) {
	Queue qs[3];
	pthread_t preader, pmunch1, pmunch2, pwriter;
	pthread_create(&preader, NULL, reader_run, &qs[0]);
	pthread_create(&pmunch1, NULL, munch1_run, &qs[1]);
	pthread_create(&pmunch2, NULL, munch2_run, &qs[2]);
	pthread_create(&pwriter, NULL, writer_run, &qs[3]);

	pthread_join(preader, NULL);
	pthread_join(pmunch1, NULL);
	pthread_join(pmunch2, NULL);
	pthread_join(pwriter, NULL);

	// TODO print stat to stderr
	return 0;
}
