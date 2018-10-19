// The collection of all standard libraries used in this program
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// error checking pthread_funcitons()
#define Pthread(what,...) if(pthread_##what(__VA_ARGS__) != 0)\
	fprintf(stderr, "pthread_"#what"() failed at %s:%d", __FILE__, __LINE__)

