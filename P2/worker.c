#include "queue.h"
#include "worker.h"

// used to convert letters to uppercase
#define UPPER_BIT 32

// TODO probably pthread_exit(???)

void *reader_run(void *q) {
	ssize_t nread;
	char *line = NULL;
	size_t len = 0;
	while ((nread = getline(&line, &len, stdin)) != -1) {
		// printf("Retrieved line of length %zu:\n", nread);
		// fwrite(line, nread, 1, stdout);

		EnqueueString((Queue*)q, line);
		// force mallocing new string for next line
		line = NULL;
		len = 0;
	}

	// tell the other workers there is no more input
	EnqueueString((Queue*)q, NULL);
	pthread_exit(NULL);
}

void *munch1_run(void *q) {
	char *line = NULL;
	// when ever there is a line
	while ((line = DequeueString((Queue*)q)) != NULL) {
		// replace all whitespaces with '*'
		for (char *c = line; *c != '\0'; c++)
			*c = isspace(*c) ? '*' : *c;
		// push to downstream
		EnqueueString((Queue*)q + 1, line);
	}
	EnqueueString((Queue*)q + 1, NULL);
	pthread_exit(NULL);
}

void *munch2_run(void *q) {
	char *line = NULL;
	// when ever there is a line
	while ((line = DequeueString((Queue*)q)) != NULL) {
		// replace all whitespaces with '*'
		for (char *c = line; *c != '\0'; c++)
			*c &= ~UPPER_BIT;
		// push to downstream
		EnqueueString((Queue*)q + 1, line);
	}
	EnqueueString((Queue*)q + 1, NULL);
	pthread_exit(NULL);
}

void *writer_run(void *q) {
	char *line = NULL;
	int nprocessed = 0;
	// when ever there is a line
	while ((line = DequeueString((Queue*)q)) != NULL) {
		// print the line and recycle
		// TODO: '\n' as '*'?
		fprintf(stdout, "%s\n", line);
		free(line);
		nprocessed = 0;
	}
	printf("EOF reached, number of string processed: %d\n", nprocessed);
	pthread_exit(NULL);
}

