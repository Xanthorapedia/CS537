#include "stdinclude.h"

typedef struct mem_node {
	struct mem_node *parent;
	struct mem_node *left_child;
	struct mem_node *right_child;
	// start of mem address
	void *start_addr;
	// end of mem address
	void *end_addr;
	// depth of left branch
	int left_depth;
	// depth of right branch
	int right_depth;
	// black node, color is 0; red node, color is 1
	// int color;
} mem_node;

mem_node *ncreate(void *start_addr, size_t size);

mem_node *nadd(mem_node *new_node, mem_node *root);

mem_node *lookup_delete(mem_node *dnode, mem_node *root);

void print_status(mem_node *node);

void *malloc537(size_t size);

void free537(void *ptr);

void *realloc537(void *ptr, size_t size);

void memcheck537(void *ptr, size_t size);

