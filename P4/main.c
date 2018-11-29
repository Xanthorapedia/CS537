#include "stdinclude.h"
#include "537malloc.h"

int main() {
	mem_node *root = NULL;

	int *a1 = malloc(sizeof(int));
	mem_node *n1 = ncreate(a1, sizeof(int));
	root = nadd(n1, root);

	int *a2 = malloc(sizeof(int));
	mem_node *n2 = ncreate(a2, sizeof(int));
	root = nadd(n2, root);

	int *a3 = malloc(sizeof(int));
	mem_node *n3 = ncreate(a3, sizeof(int));
	root = nadd(n3, root);

	int *a4 = malloc(sizeof(int));
	mem_node *n4 = ncreate(a4, sizeof(int));
	root = nadd(n4, root);

	int *a5 = malloc(sizeof(int));
	mem_node *n5 = ncreate(a5, sizeof(int));
	root = nadd(n5, root);

	int *a6 = malloc(sizeof(int));
	mem_node *n6 = ncreate(a6, sizeof(int));
	root = nadd(n6, root);

	printf("a1: %p, a2: %p, a3: %p, a4: %p, a5: %p, a6: %p\n", a1, a2, a3, a4, a5, a6);
	printf("-------------------------n1-------------------------\n");
	print_status(n1);
	printf("-------------------------n2-------------------------\n");
	print_status(n2);
	printf("-------------------------n3-------------------------\n");
	print_status(n3);
	printf("-------------------------n4-------------------------\n");
	print_status(n4);
	printf("-------------------------n5-------------------------\n");
	print_status(n5);
	printf("-------------------------n6-------------------------\n");
	print_status(n6);
	printf("------------------------root------------------------\n");
	print_status(root);

	root = lookup_delete(n2, root);
	root = lookup_delete(n4, root);

	printf("-------------------------n1-------------------------\n");
	print_status(n1);
	printf("-------------------------n3-------------------------\n");
	print_status(n3);
	printf("-------------------------n5-------------------------\n");
	print_status(n5);
	printf("-------------------------n6-------------------------\n");
	print_status(n6);
	printf("-------------------------root-------------------------\n");
	print_status(root);


}
