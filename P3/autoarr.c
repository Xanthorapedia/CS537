/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "autoarr.h"

const int DEFAULT_ARR_SIZE = 10;

void *ASARR_strip(ASArr *asarr) {
	void *ret = asarr->arr;
	free(asarr);
	return ret;
}
