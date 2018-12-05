#include "537malloc.h"

static mem_node *root = NULL;

void *_malloc537(size_t size, char *fl) {
	void *ptr = NULL;
	if (size == 0) {
		fprintf(stderr, "malloc size is 0\n");
	}
	ptr = PEONZ(malloc, size);
	ASARR_INIT(overlap, mem_node*);
	find_overlap(root, ptr, 0, overlap);
	if (ASARR_SIZE(overlap)!= 0) {
		mem_node **arr = ASARR_GET(overlap);
		for (int i = 0; i < ASARR_SIZE(overlap); i++) {
			// memory not freed
			if (arr[i]->interval.isfree != 1) {
				fprintf(stderr, "%s: double malloc\n", fl);
				exit(-1);
			}
			if (arr[i]->interval.start < ptr) {
				// case 1: first half not overlap, second half contains overlap
				arr[i]->interval.end = ptr;
			}
			else {
				// case 2: start of node is within the newly allocated mem
				PEONN(ndel, arr[i], &root);
			}
		}
	}
	mem_node *new_node = ncreate(ptr, size, 0, fl);
	PEONN(nadd, new_node, &root);
	ASARR_DESTROY(overlap);
	return ptr;
}

void _free537(void *ptr, char *fl) {
	ASARR_INIT(overlap, mem_node*);
	find_overlap(root, ptr, 0, overlap);
	int sz = ASARR_SIZE(overlap);
	if (sz != 1) {
		fprintf(stderr, "%s: address %p not allocated\n", fl, ptr);
		exit(-1);
	}
	mem_node **arr = ASARR_GET(overlap);
	if (arr[0]->interval.start != ptr) {
		fprintf(stderr, 
			"%s: address %p is not the starting address of the address block\n",
			fl, ptr);
		exit(-1);
	}
	if (arr[0]->interval.isfree == 1) {
		fprintf(stderr, "%s: double free in address %p\n", fl, ptr);
		exit(-1);
	}
	PEONN(ndel, arr[0], &root);
	free(ptr);
	ASARR_DESTROY(overlap);
}

void *_realloc537(void *ptr, size_t size, char *fl) {
	void *ret = NULL;
	if (ptr == NULL) {
		ret = _malloc537(size, fl);
	}
	else {
		if (size == 0) {
			_free537(ptr, fl);
		}
		else {
			ASARR_INIT(overlap, mem_node*);
			find_overlap(root, ptr, 0, overlap);
			mem_node **arr = ASARR_GET(overlap);
			PEONN(ndel, arr[0], &root);
			ret = PEONZ(realloc, ptr, size);
			mem_node *new_node = ncreate(ret, size, 0, fl);
			PEONN(nadd, new_node, &root);
			ASARR_DESTROY(overlap);
		}
	}
	return ret;
}

void _memcheck537(void *ptr, size_t size, char *fl) {
	ASARR_INIT(overlap, mem_node*);
	find_overlap(root, ptr, size, overlap);
	if (ASARR_SIZE(overlap) != 1) {
		fprintf(stderr, "%s: the memory is not found\n", fl);
		exit(-1);
	}
	mem_node **arr = ASARR_GET(overlap);
	if (arr[0]->interval.start <= ptr && arr[0]->interval.end >= ptr + size) {
		if (arr[0]->interval.isfree == 1) {
			fprintf(stderr, "%s: the memory has been freed\n", fl);
			exit(-1);
		}
	}
	else {
		fprintf(stderr, "%s: the memory is not found\n", fl);
		exit(-1);
	}
	ASARR_DESTROY(overlap);
}
