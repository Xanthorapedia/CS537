#include "stdinclude.h"
#include "autoarr.h"
#include "memavl.h"

void randarr(long seed, int N, int arr[], int len) {
	char *check = calloc(N, sizeof(char));
	if (seed > 0)
		srand(seed);
	for (int i = 0; i < len; i++) {
		do {
			arr[i] = rand() % N;
		} while (check[arr[i]] == 1);
		check[arr[i]] = 1; 
	}
	free(check);
}


int rand_add_del_test(int TESTN, int MAXAD, long seed) {
	int ins[TESTN], del[TESTN];
	int addr[TESTN];

	printf("Seed: %lu\n", seed);

	// generate samples (shuffling randomly generated addresses)
	printf("Addresses: ");
	randarr(seed, MAXAD, addr, TESTN);
	for (int i = 0; i < TESTN; i++)
		printf("0x%x ", addr[i]);
	printf("\n");

	printf("Insertion order: ");
	randarr(-1, TESTN, ins, TESTN);
	for (int i = 0; i < TESTN; i++)
		printf("0x%x ", addr[ins[i]]);
	printf("\n");

	printf("Deletion  order: ");
	randarr(-1, TESTN, del, TESTN);
	for (int i = 0; i < TESTN; i++)
		printf("0x%x ", addr[del[i]]);
	printf("\n");

	// repeatedly insert
	mem_node *root = NULL;
	for (int i = 0; i < TESTN; i++) {
		printf("Add 0x%x:\n", addr[ins[i]]);
		nadd(ncreate((void *) (long) addr[ins[i]], 1, 0, 0, NULL), &root);
		// change to 1 to print tree in each step
		check_tree(root, 0);
	}
	// and delete
	for (int i = 0; i < TESTN; i++) {
		printf("Del 0x%x:\n", addr[del[i]]);
		ndel(ncreate((void *) (long) addr[del[i]], 1, 0, 0, NULL), &root);
		// change to 1 to print tree in each step
		check_tree(root, 0);
	}
	return 0;
}

int find_interval_test(int TESTN, int MAXAD, long seed) {
	int ins[TESTN];
	int addr[TESTN];

	printf("Seed: %lu\n", seed);
	randarr(seed, MAXAD, addr, TESTN);
	
	// generate samples (shuffling randomly generated addresses)
	mem_node *root = NULL;
	printf("Addresses: ");
	randarr(-1, TESTN, ins, TESTN);
	for (int i = 0; i < TESTN; i++) {
		nadd(ncreate((void *) (long) addr[ins[i]], 0, 0, 0, NULL), &root);
		printf("0x%x ", addr[ins[i]]);
	}
	printf("\n");

	// prints tree
	check_tree(root, 1);

	// the list to record overlaps
	ASARR_INIT(overlap, mem_node *);
	for (int i = 0; i < TESTN; i++) {
		// generate appropriate intervals
		int start = rand() % MAXAD;
		while (start == MAXAD - 1)
			start = rand() % MAXAD;
		int size  = 0;//rand() % (MAXAD - start - 1) + 1;
		printf("Interval in question: [0x%x ,0x%x)\n", start, start + size);

		// search
		find_overlap(root, (void *) (long) start, size, overlap);

		// display results
		printf("Found: ");
		for (int i = 0; i < ASARR_SIZE(overlap); i++) {
			mem_node *node = ASARR_GET(overlap)[i];
			printf("[%p ,%p) ", node->interval.start, node->interval.end);
		}
		printf("\n");

		// dump old results
		ASARR_EMPTY(overlap);
	}

	// clean up
	for (int i = 0; i < TESTN; i++) {
		ndel(root, &root);
	}
	ASARR_DESTROY(overlap)
	return 0;
}

int main() {
//	rand_add_del_test(2048, 2048, time(NULL));
	find_interval_test(10, 10, time(NULL));
}
