#include "stdinclude.h"
#include "537malloc.h"

int main() {
	int *a = malloc(sizeof(int));
	mem_node *root = ncreate(a, sizeof(int));
	nadd(root, NULL);
	int *a1 = malloc(sizeof(int));
	mem_node *n1 = ncreate(a1, sizeof(int));
	nadd(n1, root);
	int *a2 = malloc(sizeof(int));
	mem_node *n2 = ncreate(a2, sizeof(int));
	nadd(n2, root);
	int *a3 = malloc(sizeof(int));
	mem_node *n3 = ncreate(a3, sizeof(int));
	nadd(n3, root);
	fprintf(stderr, "a: %p, a1: %p, a2: %p, a3: %p\n", a, a1, a2, a3);
	print_status(root);
	print_status(n1);
	print_status(n2);
	print_status(n3);
}
