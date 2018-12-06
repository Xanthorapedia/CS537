/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "utils.h"

void *perr_neq(int cond, char *msg, void *ret) {
	if ((cond == 0) ^ (ret == NULL))
		fprintf(stderr, "%s", msg);
	return ret;
}

