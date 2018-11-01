/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "stdinclude.h"
#include "parser.h"
#include "goal.h"
#include "utils.h"
#define OPTSTRING "f:"
#define DEFAULT_PATH "makefile"

int main(int argc, char *argv[]) {
	char opt;
	char *makefile_path = NULL;
	char **goals = NULL;
	int unrecg_arg_count = 0;
	while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
		switch (opt) {
			case 'f':
				makefile_path = optarg;
				break;
			default:
				return -1;
		}
	}
	while (optind < argc) {
		if (unrecg_arg_count == 0) {
			goals = PEONZ(calloc, argc - optind, sizeof(char*));
		}
		goals[unrecg_arg_count] = argv[optind];
		optind++;
		unrecg_arg_count++;
	}
	if (makefile_path == NULL) {
		makefile_path = DEFAULT_PATH;
	}
	if (access(makefile_path, R_OK) != 0) {
		fprintf(stderr, "Cannot read file %s\n", makefile_path);
		return -1;
	}
	PGoal_t *mgoals = NULL;
	size_t nmgoals = 0;
	size_t nobj = 0;
	mparse(makefile_path, &mgoals, &nmgoals, &nobj);
	mresovle(&mgoals, &nmgoals, nobj);
	for (size_t i = 0; i < nmgoals; i++) {
		gprint(mgoals[i]);
	}
	for (size_t i = 0; i < nmgoals; i++) {
		gdestroy(mgoals[i]);
	}
	free(mgoals);
	free(goals);
	return 0;
}

