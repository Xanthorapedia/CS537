#include "queue.h"
#include "worker.h"

#define Pthread(what,...) assert(pthread_##what(__VA_ARGS__) == 0)

// string fetching function
typedef char *(*fetch_func)(Queue *);

// string processing function
typedef char *(*process_func)(char *);

// string dispatching function
typedef void (*dispatch_func)(Queue *, char *);

// used to convert letters to uppercase
#define UPPER_BIT 32
#define MUNCH2_C '*'

// TODO probably pthread_exit(???)

// for storing thread specific processed stirng number
pthread_key_t nprocessed_key;

// a template worker function
inline static void gen_worker_run(Queue**, fetch_func, process_func, dispatch_func);

// insteade of fetching from queue, get a line from stdin
inline static char *fetchline(Queue *q);

// replace all whitespaces with MUNCH2_C
inline static char *replacew(char *line);

// capitalize all characters
inline static char *capitalize(char *line);

// print line before termination
// print number of string processed after
inline static void display(Queue* q, char *line);

// thread workers
void *reader_run(void *qs) {
	gen_worker_run(qs, fetchline, NULL, EnqueueString);
	return NULL;
}

void *munch1_run(void *qs) {
	gen_worker_run(qs, DequeueString, replacew, EnqueueString);
	return NULL;
}

void *munch2_run(void *qs) {
	gen_worker_run(qs, DequeueString, capitalize, EnqueueString);
	return NULL;
}

void *writer_run(void *qs) {
	// initialize number of processed string to 0
	// and have it automatically freed afterward
	Pthread(key_create, &nprocessed_key, free);
	Pthread(setspecific, nprocessed_key, calloc(1, sizeof(int)));
	gen_worker_run(qs, DequeueString, NULL, display);
	return NULL;
}

inline static void gen_worker_run(Queue **qs,
		fetch_func fetch, process_func process, dispatch_func dispatch) {
	char *line = NULL;
	// when ever there is a line
	while ((line = fetch(qs[0])) != NULL) {
		// process and dispatch if necessary
		if (process)
			line = process(line);
		if (dispatch)
			dispatch(qs[1], line);
	}
	// tell the downstream workers that input is depleted
	if (dispatch)
		dispatch(qs[1], NULL);
	pthread_exit(NULL);
}

inline static char *fetchline(Queue *q) {
	(void)(q);
	char *line = NULL;
	size_t len = 0;
	return (getline(&line, &len, stdin) == -1) ? NULL : line;
}

inline static char *replacew(char *line) {
	for (char *c = line; *c != '\0'; c++)
		*c = isspace(*c) ? MUNCH2_C : *c;
	return line;
}

inline static char *capitalize(char *line) {
	for (char *c = line; *c != '\0'; c++)
		*c = isalpha(*c) ? *c & ~UPPER_BIT : *c;
	return line;
}

inline static void display(Queue* q, char *line) {
	(void) q;
	// fetch previously stored value
	int *nprocessed = pthread_getspecific(nprocessed_key);
	if (line) {
		fprintf(stdout, "%s\n", line);
		free(line);
		(*nprocessed)++;
	}
	else {
		// prints count and destroy key (nprocessed automatically freed)
		printf("EOF reached, number of string processed: %d\n", *nprocessed);
		Pthread(key_delete, nprocessed_key);
	}
}

