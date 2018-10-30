/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "stdinclude.h"
#include "parser.h"
#include "goal.h"

int main(int argc, char *argv[]) {
	(void)(argc), (void)(argv);
	PGoal_t *mgoals = NULL;
	size_t nmgoals = 0;
	size_t nobj = 0;
	mparse("MakefileTest", &mgoals, &nmgoals, &nobj);
	mresovle(&mgoals, &nmgoals, nobj);
	for (size_t i = 0; i < nmgoals; i++) {
		gprint(mgoals[i]);
	}
	for (size_t i = 0; i < nmgoals; i++) {
		gdestroy(mgoals[i]);
	}
	free(mgoals);
	return 0;
}

