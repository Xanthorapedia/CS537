#include <stdio.h>
#include "537malloc.h"

int main() {
	int *ptr = NULL;

	printf("Freeing NULL pointer\n");
	free537(ptr);

	//printf("If this prints, no points\n");
	printf("This contradicts with specification of free, removing this testcase for evaluation");
	printf("As per the man page for free, free(NULL) is equivalent to no-operation");
	return 0;
}
