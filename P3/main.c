/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "stdinclude.h"
#include "autoarr.h"
#include "parser.h"
#include "goal.h"
#include "utils.h"
#define OPTSTRING "f:"
#define DEFAULT_PATH "makefile"

int main(int argc, char *argv[]) {
	char opt;
	char *makefile_path = NULL;
	while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
		switch (opt) {
			case 'f':
				makefile_path = optarg;
				break;
			default:
				return -1;
		}
	}
	if (makefile_path == NULL) {
		makefile_path = DEFAULT_PATH;
	}
	if (access(makefile_path, R_OK) != 0) {
		fprintf(stderr, "Cannot read file %s\n", makefile_path);
		return -1;
	}
	ASARR_INIT(goals, PGoal_t);
	if (mparse(makefile_path, goals) != 0) {
		return 1;
	}
	if (ASARR_SIZE(goals) == 0) {
		fprintf(stderr, "537make: *** No targets.  Stop.");
		return 1;
	}
	char *appointed = optind < argc ? argv[optind] : ASARR_GET(goals)[0]->name;
//	for (int i = 0; i < nmgoals; i++) {
//		PGoal_t goal = ASARR_GET(goals)[i];
//		gprint(goal);
//	}
	ASARR_INIT(ulist, PGoal_t);
	if (mcheckupdate(appointed, goals, ulist) != 0)
		return -1;
	int cmdexecd = 0;
	for (int i = 0; i < ASARR_SIZE(ulist); i++) {
		PGoal_t goal = ASARR_GET(ulist)[i];
		cmdexecd += goal->ncmd;
		//gprint(goal);
		int ret = 0;
		if ((ret = gupdt(goal)) != 0) {
			fprintf(stderr, "537make: *** [%s] Error %d\n", appointed, ret);
			return 1;
		}
	}
	if (cmdexecd == 0) {
		fprintf(stdout, "537make: '%s' is up to date.\n", appointed);
	}
	for (int i = 0; i < ASARR_SIZE(goals); i++) {
		gdestroy(ASARR_GET(goals)[i]);
	}
	ASARR_DESTROY(ulist);
	ASARR_DESTROY(goals);
	return 0;
}

