#include "utils.h"
#include "stdinclude.h"

const size_t DEFAULT_ARR_SIZE = 10;

void *perr_neq(int cond, char *msg, void *ret) {
	if ((cond == 0) ^ (ret == NULL))
		fprintf(stderr, "%s", msg);
	return ret;
}

