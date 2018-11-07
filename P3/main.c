/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "stdinclude.h"
#include "autoarr.h"
#include "parser.h"
#include "goal.h"
#include "utils.h"
#define OPTSTRING "f:"
char * const DEFAULT_PATHS[] = { "makefile", "Makefile" };

int main(int argc, char *argv[]) {
	char opt;
	char *makefile_path = NULL;
	while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
		switch (opt) {
			case 'f':
				makefile_path = optarg;
				if (access(makefile_path, R_OK) != 0) {
					fprintf(stderr, "537make: Cannot access specified file '%s'.\n", makefile_path);
					return -1;
				}
				break;
			default:
				return -1;
		}
	}
	// search for default file names if not specified
	if (makefile_path == NULL) {
		for (int i = 0; i < 2; i++) {
			if (access(DEFAULT_PATHS[i], R_OK) == 0) {
				makefile_path = DEFAULT_PATHS[i];
				break;
			}
		}
		if (makefile_path == NULL) {
			fprintf(stderr, "537make: No 'makefile' or 'Makefile' found.\n");
			return -1;
		}
	}
	// parse files
	ASARR_INIT(goals, PGoal_t);
	if (mparse(makefile_path, goals) != 0) {
		fprintf(stderr, "537make: *** Bad makefile.  Stop.");
		return 1;
	}
	if (ASARR_SIZE(goals) == 0) {
		fprintf(stderr, "537make: *** No targets.  Stop.");
		return 1;
	}
	// select target and exec
	char *appointed = optind < argc ? argv[optind] : ASARR_GET(goals)[0]->name;
	ASARR_INIT(ulist, PGoal_t);
	if (mcheckupdate(appointed, goals, ulist) != 0) {
		fprintf(stderr, "537make: *** Bad makefile.  Stop.");
		return 1;
	}
	int cmdexecd = 0;
	for (int i = 0; i < ASARR_SIZE(ulist); i++) {
		PGoal_t goal = ASARR_GET(ulist)[i];
		cmdexecd += goal->ncmd;
		int ret = 0;
		if ((ret = gupdt(goal)) != 0) {
			fprintf(stderr, "537make: *** [%s] Error %d\n", appointed, ret);
			return 1;
		}
	}
	if (cmdexecd == 0) {
		fprintf(stdout, "537make: '%s' is up to date.\n", appointed);
	}
	// clean up
	for (int i = 0; i < ASARR_SIZE(goals); i++) {
		gdestroy(ASARR_GET(goals)[i]);
	}
	ASARR_DESTROY(ulist);
	ASARR_DESTROY(goals);
	return 0;
}

