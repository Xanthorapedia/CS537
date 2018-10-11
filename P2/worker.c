#include "queue.h"
#include "worker.h"

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

// init control for initializing the thread once
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

// key for storing thread specific data
static pthread_key_t tsd_key;

// a template worker function
inline static void gen_worker_run(Queue**, fetch_func, process_func, dispatch_func);

// insteade of fetching from queue, get a line from stdin
inline static char *fetchline(Queue *q);

// replace the '\n' at EOL with '\0'
inline static char *replacenewline(char *line);

// replace all whitespaces with MUNCH2_C
inline static char *replacew(char *line);

// capitalize all characters
inline static char *capitalize(char *line);

// initializes the thread-specific key for a thread
inline static void init_thread_specific();

// cleans up the thread-specific key
inline static void destroy_thread_specific_key(void *key);

// print line before termination
// print number of string processed after
inline static void display(Queue* q, char *line);

// thread workers
void *reader_run(void *qs) {
	gen_worker_run(qs, fetchline, replacenewline, EnqueueString);
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
	Pthread(once, &init_once, init_thread_specific);
	Pthread(setspecific, tsd_key, calloc(1, sizeof(int)));
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
	return (getline(&line, &len, stdin) == -1) ? free(line), NULL : line;
}

inline static char *replacenewline(char *line) {
	int len = strlen(line);
	if (line[len - 1] == '\n')
		line[len - 1] = '\0';
	return line;
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
	int *nprocessed = pthread_getspecific(tsd_key);
	if (line) {
		fprintf(stdout, "%s\n", line);
		free(line);
		(*nprocessed)++;
	}
	else
		fprintf(stdout, "EOF reached, number of string(s) processed: %d\n\n", *nprocessed);
}

inline static void init_thread_specific() {
	Pthread(key_create, &tsd_key, destroy_thread_specific_key);
}

inline static void destroy_thread_specific_key(void *key) {
	free(key);
	Pthread(key_delete, tsd_key);
}

