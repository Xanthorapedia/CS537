#include "stdinclude.h"

#pragma once

// max length of each string (including '\0')
extern const int BUFFER_SIZE;

/*
 * In each thread routines below, arg is an array of two Queue *'s, i.e.
 * Queue *in = arg[0];
 * Queue *out = arg[1];
 *
 * Unless explicitly speified, the routine takes a char * from 'in' and process
 * it in certain ways before pushing it into 'out'.
 */

// A Reader thread routine that reads lines from stdin
void *reader_run(void *arg);

// A Muncher thread routine that replaces whitespaces with '*''s
void *munch1_run(void *arg);

// A Muncher thread routine that capitalize all alphabetical characters
void *munch2_run(void *arg);

// A Writer thread routine that writes the line to stdout and prints stats on
// termination
void *writer_run(void *arg);

