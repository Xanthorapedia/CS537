#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *reader_run(void *arg);

void *munch1_run(void *arg);

void *munch2_run(void *arg);

void *writer_run(void *arg);

