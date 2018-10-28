#include "llist.h"

#include "stdinclude.h"

LList_t *LLcreate() {
	LList_t *list = malloc(sizeof(LList_t));
	// dummy node
	LNode_t *newnode = malloc(sizeof(LNode_t));
	*newnode = (LNode_t) { .prev = NULL, .next = NULL, .payload = NULL};
	*list = (LList_t) { .head = newnode, .tail = newnode, .size = 0 };
	return list;
}

void LLdestroy(LList_t *list) {
	LNode_t *cur = list->head;
	while (cur) {
		LNode_t *next = cur->next;
		free(cur);
		cur = next;
	}
	free(list);
}

void LLinsert(LList_t *list, void *payload) {
	LNode_t *newnode = malloc(sizeof(LNode_t));
	*newnode = (LNode_t) { .prev = list->tail, .next = NULL, .payload = payload };
	list->tail->next = newnode;
	list->tail = newnode;
	list->size++;
}

// to array
int LLtoarr(LList_t *list, void *arr) {
	void **payload = *(void **)arr = malloc(list->size * sizeof(void *));
	LNode_t *cur = list->head->next;
	for (int i = 0; cur; i++) {
		LNode_t *next = cur->next;
		payload[i] = cur->payload;		
		cur = next;
	}
	return list->size;
}

