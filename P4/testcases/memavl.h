#include "stdinclude.h"
#include "autoarr.h"

#pragma once

typedef enum { L, R, DEFAULT } side;
typedef enum { PARENT = 1, LEFTMOST = 2 } search_op;

typedef struct _mem_i {
	// start of mem address (inclusive)
	void *start;
	//   end of mem address (exclusive)
	void *end;
	// flag for whether the interval is freed, 1 is free, 0 is allocated
	int isfree;
	// debugging information: place of status change
	char *fl;
} mem_i;

typedef struct mem_node {
	// left_child := children[0], right_child := children[1]
	union {
		struct {
			struct mem_node *left_child;
			struct mem_node *right_child;
		};
		struct mem_node *children[2];
	};
	struct mem_node *parent;
	// the height of the current node
	int height;
	mem_i interval;
} mem_node;


// compares addresses in dictionary order (i.e. start, end)
int dccmp(mem_node *node0, mem_node *node1);

// You should know what those are :)
mem_node *ncreate(void *start, size_t sz, int isfree, char *fl);
void ndestroy(mem_node *node);

int nadd(mem_node *inode, mem_node **root);
int ndel(mem_node *inode, mem_node **root);
void find_overlap(mem_node *root, void *start, size_t sz, ASArr *overlap);

void print_status(mem_node *node);
void check_tree(mem_node *root, int print);

