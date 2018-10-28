#include "stdinclude.h"

#pragma once

// A doubly linked list node
typedef struct __node {
	struct __node *prev;
	struct __node *next;
	void *payload;
} LNode_t;

// A doubly linked list
typedef struct __list {
	LNode_t *head;
	LNode_t *tail;
	size_t   size;
} LList_t;

// basic operations
LList_t *LLcreate();

void LLdestroy(LList_t *list);

void LLinsert(LList_t *list, void *payload);

// to array
// returns the number of elements
int LLtoarr(LList_t *list, void *arr);

