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
#define BUFFER_SIZE 1024
#define EOFMSG \
	"=================================================\n"\
	"EOF reached, number of string(s) processed: %d\n"\
	"=================================================\n"


// TODO probably pthread_exit(???)

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
	// char *line = NULL;
	// size_t len = 0;
	// return (getline(&line, &len, stdin) == -1) ? free(line), NULL : line;
	char *buf;
	if((buf = calloc(BUFFER_SIZE, sizeof(char))) == NULL) {
		fprintf(stderr, "Cannot calloc for buffer\n");
		return NULL;
	}
	if(fgets(buf, BUFFER_SIZE, stdin) != NULL) {
    	if (strncmp(buf[BUFFER_SIZE - 2], "\0", 1) ！= 0 && 
    		buf[BUFFER_SIZE - 2] != '\n' && buf[BUFFER_SIZE - 2] != EOF) {
    		int ch;
    		int c = 0;
    		while ((ch = fgetc(stdin)) != '\n' && ch != EOF) {
    			c++;
    		}
    		if (c != 0) {
    			free(buf);
    			buf = NULL;
    			fprintf(stderr, "line exceeds buffer size\n");
    			return NULL;
    		}
    	}
    }
    return buf;
}

inline static char *replacew(char *line) {
	for (char *c = line; *c != '\0'; c++)
		*c = (*c != '\t' && *c != '\n' && isspace(*c)) ? MUNCH2_C : *c;
	return line;
}

inline static char *capitalize(char *line) {
	for (char *c = line; *c != '\0'; c++)
		*c = isalpha(*c) ? *c & ~UPPER_BIT : *c;
	return line;
}

inline static void display(Queue* q, char *line) {
	(void) q;
	// this value is stored in the thread-local space
	static __thread int nprocessed = 0;
	if (line) {
		fprintf(stdout, "%s", line);
		free(line);
		nprocessed++;
	}
	else
		fprintf(stdout, EOFMSG, nprocessed);
}

