#include "537malloc.h"
#include "utils.h"


int max_depth(mem_node *node) {
	if (node->left_depth > node->right_depth) {
		return node->left_depth;
	}
	else {
		return node->right_depth;
	}
}

void update_depth(mem_node *leaf) {
	mem_node *node = leaf;
	while(node->parent != NULL) {
		node = node->parent;
		if (node->left_child != NULL) {
			node->left_depth = max_depth(node->left_child) + 1;
		}
		else {
			node->left_depth = 0;
		}
		if (node->right_child != NULL) {
			node->right_depth = max_depth(node->right_child) + 1;
		}
		else {
			node->right_depth = 0;
		}
	}
}

void set_r (mem_node *node) {
	node->color = 1;
}

void set_b (mem_node *node) {
	node->color = 0;
}

void rotate_l (mem_node *node, int mode) {
	if (mode != 0 && mode != 1) {
		fprintf(stderr, "invalid mode\n");
		exit(-1);
	}
	mem_node *curr = node;
	mem_node *parent = curr->parent;
	mem_node *gparent = parent->parent;
	if (mode == 0) {
		gparent->right_child = parent->left_child;
		parent->left_child = gparent;
		parent->parent = gparent->parent;
		gparent->parent = parent;
	}
	else {
		curr->parent = gparent;
		gparent->left_child = curr;
		parent->right_child = curr->left_child;
		parent->parent = curr;
		curr->left_child = parent;
	}
}

void rotate_r (mem_node *node, int mode) {
	if (mode != 0 && mode != 1) {
		fprintf(stderr, "invalid mode\n");
		exit(-1);
	}
	mem_node *curr = node;
	mem_node *parent = curr->parent;
	mem_node *gparent = parent->parent;
	if (mode == 0) {
		gparent->left_child = parent->right_child;
		parent->right_child = gparent;
		parent->parent = gparent->parent;
		gparent->parent = parent;
	}
	else {
		curr->parent = gparent;
		gparent->right_child = curr;
		parent->left_child = curr->right_child;
		parent->parent = curr;
		curr->right_child = parent;
	}
}

void nrestruct(mem_node *new_node) {
	mem_node *curr = new_node;
	mem_node *parent;
	while ((parent = curr->parent) != NULL && parent->color == 1) {
		mem_node *gparent = parent->parent;
		// parent is left child of grand parent
		if (gparent->left_child == parent) {
			// 1: when both parent and uncle are RED, recolor. make both parent and
			// uncle node BLACK and the grand parent node RED
			if (gparent->right_child != NULL && gparent->right_child->color == 1) {
				set_b(gparent->right_child);
				set_b(gparent->left_child);
				set_r(gparent);
				curr = gparent;
			}
			else {
				// 2: when uncle node is BLACK, and current node is the right child of
				// parent node, left rotate
				if (parent->right_child != NULL && parent->right_child == curr) {
					rotate_l(curr, 1);
					curr = parent;
				}
				// 3: when uncle node is BLACK, and current node is the left child of
				// parent node, set parent to BLACK and grand parent to RED, and right 
				// rotate
				else if (parent->left_child != NULL && parent->left_child == curr) {
					set_b(parent);
					set_r(gparent);
					rotate_r(curr, 0);
				}
			}
		}
		// parent is the right child of grand parent
		else if (gparent->right_child == parent) {
			// 1: when both parent and uncle are RED, recolor. make both parent and
			// uncle node BLACK and the grand parent node RED
			if (gparent->left_child != NULL && gparent->left_child->color == 1) {
				set_b(gparent->right_child);
				set_b(gparent->left_child);
				set_r(gparent);
				curr = gparent;
			}
			else {
				// 2: when uncle node is BLACK, and current node is the left child of
				// parent node, left rotate
				if (parent->left_child != NULL && parent->left_child == curr) {
					rotate_l(curr, 1);
					curr = parent;
				}
				// 3: when uncle node is BLACK, and current node is the right child of
				// parent node, set parent to BLACK and grand parent to RED, and right 
				// rotate
				else if (parent->right_child != NULL && parent->right_child == curr) {
					set_b(parent);
					set_r(gparent);
					rotate_r(curr, 0);
				}
			}
		}
		else {
			fprintf(stderr, "None of gparent child is parent, should not happen\n");
			exit(-1);
		}
	}
	// set root to black
	curr = new_node;
	while (curr->parent != NULL) {
		curr = curr->parent;
	}
	set_b(curr);
}

mem_node *ncreate(void *start_addr, size_t size) {
	mem_node *new_node = PEONZ(malloc, sizeof(mem_node));
	new_node->parent = NULL;
	new_node->left_child = NULL;
	new_node->right_child = NULL;
	new_node->start_addr = (void*)start_addr;
	new_node->end_addr = (void*)(new_node->start_addr) + size;
	new_node->left_depth = 0;
	new_node->right_depth = 0;
	new_node->color = 1;
	return new_node;
}

void nadd(mem_node *new_node, mem_node *root) {
	// when root is null, the red-black tree is not created yet
	if (root == NULL) {
		root = new_node;
		root->color = 0;
		return;
	}
	// when root is not null, there is the tree already
	mem_node *curr = root;
	while (curr != NULL) {
		// new node belong to left part
		if (new_node->end_addr < curr->start_addr) {
			if (curr->left_child != NULL) {
				curr = curr->left_child;
				continue;
			}
			else {
				curr->left_child = new_node;
				new_node->parent = curr;
				update_depth(new_node);
				break;
			}
		}
		// new node belong to right part
		else if (new_node->start_addr > curr->end_addr) {
			if (curr->right_child != NULL) {
				curr = curr->right_child;
				continue;
			}
			else {
				curr->right_child = new_node;
				new_node->parent = curr;
				update_depth(new_node);
				break;
			}
		}
		// overlap memory area allocated, should not happen
		else {
			fprintf(stderr, "overlap memory area\n");
			exit(-1);
		}
	}
	// rebalance
	nrestruct(new_node);
	while (root->parent != NULL) {
		root = root->parent;
	}
}

// TODO: red-balck tree delete operation
void ndelete (mem_node *new_node, mem_node *root) {
	if (root == NULL) {
		fprintf(stderr, "the red-black tree is empty, nothing can be deleted\n");
		exit(-1);
	}
}

void print_status(mem_node *node) {
	printf("start_addr: %p\n", node->start_addr);
	printf("end_addr: %p\n", node->end_addr);
	printf("parent: %p\n", node->parent);
	printf("left_child: %p\n", node->left_child);
	printf("right_child: %p\n", node->right_child);
	printf("left_depth: %d\n", node->left_depth);
	printf("right_depth: %d\n", node->right_depth);
}

void *malloc537(size_t size) {
	
}

void free537(void *ptr) {

}

void *realloc537(void *ptr, size_t size) {

}

void memcheck537(void *ptr, size_t size) {

}