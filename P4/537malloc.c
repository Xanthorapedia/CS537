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

void update_depth(mem_node *new_node) {
	mem_node *node = new_node;
	while(node != NULL) {
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
		node = node->parent;
	}
}

int ncheck(mem_node *exist, mem_node *check) {
	if (exist != NULL && exist == check) {
		return 1;
	}
	else {
		return 0;
	}
}

void rotate_ll(mem_node *node) {
	mem_node *curr = node;
	mem_node *lchild = curr->left_child;
	curr->left_child = lchild->right_child;
	if (curr->left_child != NULL) {
		curr->left_child->parent = curr;
	}
	lchild->right_child = curr;
	lchild->parent = curr->parent;
	if (curr->parent != NULL) {
		if (ncheck(curr->parent->left_child, curr)) {
			lchild->parent->left_child = lchild;
		}
		else {
			lchild->parent->right_child = lchild;
		}
	}
	curr->parent = lchild;
	curr = lchild;
	lchild = NULL;
	update_depth(curr->left_child);
	update_depth(curr->right_child);
}

void rotate_lr(mem_node *node) {
	mem_node *curr = node;
	mem_node *lchild = curr->left_child;
	mem_node *gchild = lchild->right_child;
	curr->left_child = gchild;
	lchild->right_child = gchild->left_child;
	if (lchild->right_child != NULL) {
		lchild->right_child->parent = lchild;
	}
	lchild->parent = gchild;
	gchild->left_child = lchild;
	gchild->parent = curr;
	rotate_ll(curr);
}

void rotate_rr(mem_node *node) {
	mem_node *curr = node;
	mem_node *rchild = curr->right_child;
	curr->right_child = rchild->left_child;
	if (curr->right_child != NULL) {
		curr->right_child->parent = curr;
	}
	rchild->left_child = curr;
	rchild->parent = curr->parent;
	if (curr->parent != NULL) {
		if (ncheck(curr->parent->left_child, curr)) {
			rchild->parent->left_child = rchild;
		}
		else {
			rchild->parent->right_child = rchild;
		}
	}
	curr->parent = rchild;
	curr = rchild;
	rchild = NULL;
	update_depth(curr->left_child);
	update_depth(curr->right_child);
}

void rotate_rl(mem_node *node) {
	mem_node *curr = node;
	mem_node *rchild = curr->right_child;
	mem_node *gchild = rchild->left_child;
	curr->right_child = gchild;
	rchild->left_child = gchild->right_child;
	if (rchild->left_child != NULL) {
		rchild->left_child->parent = rchild;
	}
	rchild->parent = gchild;
	gchild->right_child = rchild;
	gchild->parent = curr;
	rotate_rr(curr);
}

void nrestruct(mem_node *new_node) {
	mem_node *curr = new_node;
	mem_node *child = NULL;
	mem_node *gchild = NULL;
	while (curr != NULL) {
		// if need reconstruct
		if (curr->left_depth - curr->right_depth > 1 
			|| curr->right_depth - curr->left_depth > 1) {
			if (ncheck(curr->left_child, child)) {
				// case left left
				if (ncheck(child->left_child, gchild)) {
					rotate_ll(curr);
				}
				// case left right
				else if (ncheck(child->right_child, gchild)) {
					rotate_lr(curr);
				}
				else {
					fprintf(stderr, "nonexist case\n");
					exit(-1);
				}
			}
			else if (ncheck(curr->right_child, child)) {
				// case right right
				if (ncheck(child->right_child, gchild)) {
					rotate_rr(curr);
				}
				// case right left
				else if (ncheck(child->left_child, gchild)) {
					rotate_rl(curr);
				}
				else {
					fprintf(stderr, "nonexist case\n");
					exit(-1);
				}
			}
			else {
				fprintf(stderr, "nonexist case\n");
				exit(-1);
			}
		}
		// balanced until now
		else {
			gchild = child;
			child = curr;
			curr = curr->parent;
		}
	}
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
	return new_node;
}

void ndestroy(mem_node *node) {
	free(node);
}

mem_node *nadd(mem_node *new_node, mem_node *root) {
	// when root is null, the red-black tree is not created yet
	if (root == NULL) {
		root = new_node;
		return root;
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
	return root;
}

mem_node *ndelete(mem_node *target, mem_node *root) {
	// delete case 1: deleting node do not have successors
	mem_node *parent = target->parent;
	if (target->left_child == NULL && target->right_child == NULL) {
		// deleting node is the only node
		if (parent == NULL) {
			ndestroy(target);
			root = NULL;
			return root;
		}
		// deleting node is parent's LEFT child
		if (ncheck(parent->left_child, target)) {
			parent->left_child = NULL;
		}
		// deleting node is parent's RIGHT child
		else if (ncheck(parent->right_child, target)) {
			parent->right_child = NULL;
		}
		else {
			fprintf(stderr, "nonexist case\n");
			exit(-1);
		}
		ndestroy(target);
		update_depth(parent);
		nrestruct(parent);
	}
	// deleting case 2: only one RIGHT child
	else if (target->left_child == NULL && target->right_child != NULL) {
		if (parent == NULL) {
			target->right_child->parent = NULL;
			root = target->right_child;
			ndestroy(target);
			return root;
		}
		// deleting node is parent's LEFT child
		if (ncheck(parent->left_child, target)) {
			parent->left_child = target->right_child;
		}
		// deleting node is parent's RIGHT child
		else if (ncheck(parent->right_child, target)) {
			parent->right_child = target->right_child;
		}
		else {
			fprintf(stderr, "nonexist case\n");
			exit(-1);
		}
		ndestroy(target);
		update_depth(parent);
		nrestruct(parent);
	}
	// deleting case 3: only one LEFT child
	else if (target->right_child == NULL && target->left_child != NULL) {
		if (parent == NULL) {
			target->left_child->parent = NULL;
			root = target->left_child;
			ndestroy(target);
			return root;
		}
		// deleting node is parent's LEFT child
		if (ncheck(parent->left_child, target)) {
			parent->left_child = target->left_child;
		}
		// deleting node is parent's RIGHT child
		else if (ncheck(parent->right_child, target)) {
			parent->right_child = target->left_child;
		}
		else {
			fprintf(stderr, "nonexist case\n");
			exit(-1);
		}
		ndestroy(target);
		update_depth(parent);
		nrestruct(parent);
	}
	// deleting case 4: has both child
	else if (target->right_child != NULL && target->left_child != NULL) {
		mem_node *curr = target->right_child;
		while (curr->left_child != NULL) {
			curr = curr->left_child;
		}
		target->start_addr = curr->start_addr;
		target->end_addr = curr->end_addr;
		ndelete(curr, root);
	}
	else {
		fprintf(stderr, "nonexist case\n");
		exit(-1);
	}
	while (root->parent != NULL) {
		root = root->parent;
	}
	return root;
}

// TODO: red-balck tree delete operation
mem_node *lookup_delete(mem_node *dnode, mem_node *root) {
	if (root == NULL) {
		fprintf(stderr, "the balanced tree is empty, nothing can be deleted\n");
		exit(-1);
	}
	mem_node *curr = root;
	mem_node *target = NULL;
	// find deleting node
	while (curr->start_addr != dnode->start_addr) {
		// new node belong to left part
		if (dnode->end_addr < curr->start_addr) {
			curr = curr->left_child;
			if (curr == NULL) {
				fprintf(stderr, "cannot find deleting node\n");
				exit(-1);
			}
		}
		// new node belong to left part
		else if (dnode->start_addr > curr->end_addr) {
			curr = curr->right_child;
			if (curr == NULL) {
				fprintf(stderr, "cannot find deleting node\n");
				exit(-1);
			}
		}
		else {
			fprintf(stderr, "cannot find deleting node\n");
			exit(-1);
		}
	}
	target = curr;
	if (target->end_addr != dnode->end_addr) {
		fprintf(stderr, "cannot find deleting node\n");
		exit(-1);
	}
	root = ndelete(target, root);
	return root;
}

void print_status(mem_node *node) {
	if (node == NULL) {
		printf("node is NULL\n");
		printf("----------------------------------------------------\n");
	}
	printf("start_addr: %p\n", node->start_addr);
	printf("end_addr: %p\n", node->end_addr);
	printf("parent: %p\n", node->parent);
	printf("left_child: %p\n", node->left_child);
	printf("right_child: %p\n", node->right_child);
	printf("left_depth: %d\n", node->left_depth);
	printf("right_depth: %d\n", node->right_depth);
	printf("----------------------------------------------------\n");
}

void *malloc537(size_t size) {
	
}

void free537(void *ptr) {

}

void *realloc537(void *ptr, size_t size) {

}

void memcheck537(void *ptr, size_t size) {

}
