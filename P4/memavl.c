#include "memavl.h"
#include "autoarr.h"
#include "utils.h"

#define max(x, y) ((x) > (y) ? (x) : (y))

// makes the "child" become a "lr" child of "parent"
inline static void adopt(mem_node *parent, mem_node *child, side lr);

// balance factor of the node (r-l)
inline static int bfactor(mem_node *node);

// always goes to left
int lmcmp(mem_node *node0, mem_node *node1);

// non-overlapping comparison
// return value < 0 if node0 preceeds node1; > 0 if the reverse; = 0 if overlapping
int nocmp(mem_node *node0, mem_node *node1);

// rebalances the tree from "new_node" up
static mem_node *restruct(mem_node *new_node);

// rotates around "pivot" in the side specified by "op"
static mem_node *rotate(mem_node *pivot, side sid);

// one more than the greater of children's height, 0 if no children
inline static int update_height(mem_node *node);

// determines which side is the node wrt its parent
inline static side whichside(mem_node *node);

mem_node *search(mem_node *inode, mem_node *root, search_op op, side *sid) {
	int (*cmp)(mem_node *, mem_node *) = dccmp;
	if (op & LEFTMOST)
		cmp = lmcmp;
	mem_node *ret = NULL;
	// the side of curr relative to its parent
	side lr = 0;
	for (mem_node *test = root; test != NULL && lr != DEFAULT;
			ret = test, test = test->children[lr]) {
		// determine the side to go
		if (cmp(inode, test) < 0)
			lr = L;
		else if (cmp(inode, test) > 0)
			lr = R;
		else
			lr = DEFAULT;
	}
	if (sid != NULL)
		*sid = lr;
	return ret;
}

mem_node *ncreate(void *start, size_t sz, int isfree, char *fl) {
	mem_node *new_node = PEONZ(malloc, sizeof(mem_node));
	*new_node = (mem_node) {
		.parent = NULL,
		.children = { NULL },
		.interval = { start, start + sz, isfree, fl },
		.height = 0
	};
	return new_node;
}

int nadd(mem_node *inode, mem_node **root) {
	// when root is null, the tree is not created yet
	if (*root == NULL) {
		*root = inode;
		return 0;
	}

	// search for leaf and append to the appropriate side
	side sid = DEFAULT;
	mem_node *parent = search(inode, *root, 0, &sid);
	// overlap memory area allocated, should not happen
	if (nocmp(parent, inode) == 0) {
		fprintf(stderr, "overlap memory area\n");
		return -1;
	}
	adopt(parent, inode, sid);

	// rebalance
	*root = restruct(inode);
	return 0;
}

int ndel(mem_node *inode, mem_node **root) {
	if (*root == NULL) {
		fprintf(stderr, "cannot delete from an empty tree\n");
		return -1;
	}

	// locate victim and find its side
	side sid = DEFAULT;
	mem_node *victim = search(inode, *root, 0, &sid);
	if (victim == NULL) {
		fprintf(stderr, "cannot delete non-existent node\n");
		return -1;
	}

	// find heir to replace victim
	// as well as the point to start restruction (whose subtree has changed)
	mem_node *heir, *restruct_s;
	if (victim->left_child == NULL || victim->right_child == NULL) {
		heir = victim->left_child == NULL ?
			victim->right_child : victim->left_child;
		restruct_s = victim->parent;
		adopt(victim->parent, heir, whichside(victim));
	}
	else {
		side lr = DEFAULT;
		heir = search(inode, victim->right_child, LEFTMOST, &lr);
		// will eventually hit where victim was, just update height before that
		restruct_s = heir->parent;
		// detach heir from its parent
		adopt(heir->parent, heir->right_child, whichside(heir));
		// here just copying the heir data to the victim is fine
		victim->interval = heir->interval;
		victim = heir;
	}

	// rebalance
	if (victim == *root)
		*root = heir;
	else
		*root = restruct(restruct_s);

	ndestroy(victim);

	return 0;
}

void find_overlap(mem_node *root, void *start, size_t sz, ASArr *overlap) {
	// base case
	if (root == NULL)
		return;

	// restore type
	ASARR_DEFTYPE(overlap, mem_node *);
	mem_node inode = { .interval = { start, start + sz, 0, NULL } };
	int cmp = nocmp(root, &inode);

	// if root < inode, no need to search the left tree
	if (cmp >= 0)
		find_overlap(root->left_child, start, sz, overlap);
	if (cmp == 0)
		ASARR_INSERT(overlap, root);
	if (cmp <= 0)
		find_overlap(root->right_child, start, sz, overlap);
}

/////////////////////////////// helper methods ////////////////////////////////

inline static void adopt(mem_node *parent, mem_node *child, side lr) {
	// in case parent == root
	if (parent != NULL)
		parent->children[lr] = child;
	if (child != NULL)
		child->parent = parent;
}

inline static int bfactor(mem_node *node) {
	int lh = node && node->left_child  ? node->left_child->height  : -1;
	int rh = node && node->right_child ? node->right_child->height : -1;
	return rh - lh;
}

int dccmp(mem_node *node0, mem_node *node1) {
	int start = node0->interval.start - node1->interval.start;
	return start == 0 ? node0->interval.end - node1->interval.end : start;
}

int lmcmp(mem_node *node0, mem_node *node1) {
	(void) node0, (void) node1;
	return -1;
}

int nocmp(mem_node *node0, mem_node *node1) {
	// returns 0 (overlap) if is of the pattern "start start end end"
	// -1 if node0 preceeds node1, 1 otherwise
	if (node1->interval.start == node0->interval.start 
		&& node1->interval.end == node0->interval.start) {
		return 0;
	}
	return (node1->interval.end <= node0->interval.start)
		 - (node0->interval.end <= node1->interval.start);
}

void ndestroy(mem_node *node) {
	free(node);
}

static mem_node *restruct(mem_node *new_node) {
	mem_node *new_root = new_node;
	for (mem_node *curr = new_node; curr != NULL; curr = curr->parent) {
		int curr_bf = bfactor(curr);
		// < -1, > 0 -> lr; < -1, < 0 -> ll
		if (curr_bf < -1) {
			if (bfactor(curr->left_child) > 0)
				rotate(curr->left_child->right_child, L);
			curr = rotate(curr->left_child, R);
		}
		// >  1, > 0 -> rr; >  1, < 0 -> rl
		else if (curr_bf > 1) {
			if (bfactor(curr->right_child) < 0)
				rotate(curr->right_child->left_child, R);
			curr = rotate(curr->right_child, L);
		}
		update_height(curr);
		new_root = curr;
	}
	return new_root;
}

static mem_node *rotate(mem_node *pivot, side sid) {
	if (sid == DEFAULT) {
		fprintf(stderr, "Warning: invalid rotation side");
		return pivot;
	}
	// suppose we are doing r-rotation
	side dis = 1 - sid;
	mem_node *self = pivot;
	mem_node *mychild  = self->children[sid];
	mem_node *myparent = self->parent;
	mem_node *mygrandparent = myparent->parent;
	// my grandparent will adopt me as whatever-child
	adopt(mygrandparent, self, whichside(myparent));
	// my parent will adopt my r-child as l-child
	adopt(myparent, mychild, dis);
	// and I will adopt my parent as r-child
	adopt(self, myparent, sid);

	update_height(myparent);
	update_height(self);

	return self;
}

inline static side whichside(mem_node *node) {
	if (node->parent == NULL) return DEFAULT;
	else if (node == node->parent->left_child)  return L;
	else if (node == node->parent->right_child) return R;
	else return DEFAULT;
}

inline static int update_height(mem_node *node) {
	int max = -1;
	if (node->left_child != NULL)
		max = max(max, node->left_child->height);
	if (node->right_child != NULL)
		max = max(max, node->right_child->height);
	max++;
	node->height = max;
	return max;
}

void print_status(mem_node *node) {
	if (node == NULL)
		printf("<EMPTY NODE>\n");
	printf("[%p, %p) %c @ %p (lvl %i) from %s\n",
			node->interval.start, node->interval.end,
			node->interval.isfree ? 'f' : 'a', node,
			node->height, node->interval.fl);
}

void check_tree_util(mem_node *root, char* line, int print) {
	int indent = strlen(line);
	static const char LR[] = "LR";
	static const char *ind[] = { "|   ", "    " };
	for (int i = 0; i < 2; i++)
		if (root->children[i] != NULL) {
			mem_node *child = root->children[i];
			if (print) {
				printf("%s+-%c=", line, LR[i]);
				print_status(child);
				strcat(line, ind[i]);
			}
			if (	// link not bi-directional
					child->parent != root ||
					// wrong hierarchy
					(i == L) != (dccmp(child, child->parent) < 0) ||
					// overlapping
					nocmp(child, child->parent) == 0
			   ) {
				if (print)
					fprintf(stderr, "%s^ Warning: bad node\n", line);
				else
					fprintf(stderr, "Warning: bad node:"), print_status(child);
			}
			check_tree_util(child, line, print);
			line[indent] = '\0';
		}
}

void check_tree(mem_node *root, int print) {
	if (root == NULL) {
		printf("<empty tree>\n");
		return;
	}
	char *indent = malloc(4 * root->height * sizeof(char) + 1);
	indent[0] = '\0';
	if (print)
		print_status(root);
	check_tree_util(root, indent, print);
	free(indent);
}

