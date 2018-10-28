/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "stdinclude.h"
#include "parser.h"

int main(int argc, char *argv[]) {
	(void)(argc), (void)(argv);
	PGoal_t *mgoals = NULL;
	int nmgoals = 0;
	mparse("MakefileTest", &mgoals, &nmgoals);
	return 0;
}

