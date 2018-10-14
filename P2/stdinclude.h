// The collection of all standard libraries used in this program
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// assert() wrapped pthread_funcitons()
#define Pthread(what,...) assert(pthread_##what(__VA_ARGS__) == 0)

