#include "goal.h"
#include "stdinclude.h"
#include "autoarr.h"
#include "utils.h"

PCmd_t ccreate(size_t argc, char **argv) {
	PCmd_t newcmd = PEONZ(malloc, sizeof(Cmd_t));
	newcmd->argc = argc;
	newcmd->argv = argv;
	return newcmd;
}

void cdestroy(PCmd_t cmd) {
	for (size_t i = 0; i < cmd->argc; i++)
		free(cmd->argv[i]);
	free(cmd->argv);
	free(cmd);
}

PGoal_t gcreate(char *name, char **depname, PGoal_t *dep, size_t ndep, PCmd_t *cmd, size_t ncmd) {
	PGoal_t newgoal = PEONZ(malloc, sizeof(Goal_t));
	newgoal->name = name;
	if (depname != NULL)
		newgoal->depname = depname;
	else
		newgoal->dep = dep;
	newgoal->ndep = ndep;
	newgoal->cmd = cmd;
	newgoal->ncmd = ncmd;
	return newgoal;
}

void gdestroy(PGoal_t goal) {
	free(goal->name);
	free(goal->depname);
	for (size_t i = 0; i < goal->ncmd; i++)
		cdestroy(goal->cmd[i]);
	free(goal->cmd);
	free(goal);
}


void cprint(PCmd_t cmd) {
	printf("  Cmmd @ %p:", cmd);
	for (size_t i = 0; i < cmd->argc; i++) {
		printf(" \"%s\"", cmd->argv[i]);
	}
	printf("\n");
}

void gprint(PGoal_t goal) {
	printf("Goal @ %p:\n", goal);
	printf("name: %s\n", goal->name);
	printf("%lu dependencies:\n", goal->ndep);
	for (size_t i = 0; i < goal->ndep; i++)
		printf("  Goal @ %p: \"%s\"\n", goal->dep[i], goal->dep[i]->name);
	printf("%lu comamnds:\n", goal->ncmd);
	for (size_t i = 0; i < goal->ncmd; i++) {
		cprint(goal->cmd[i]);
	}
	printf("\n");
}

/* store all the goals into a hash table, for all the dependencies
   mentioned in the file, find them in hash table. If successfully
   found, replace the name of the goal to a goal struct. If the goal
   cannot be found, print the error message and return -1
   param: goals: goal array
          ngoals: length of goals
 */
int mresovle(PGoal_t *goallist[], size_t *ng, size_t nobjs) {
	PGoal_t *goals = *goallist;
	size_t ngoals = *ng;

	// finalized goals (also include file dependencies)
	ASARR_INIT(finalized, PGoal_t);

	// create hash table
	struct hsearch_data ht = (struct hsearch_data) { 0 };
	PEONZ(hcreate_r, nobjs * 2, &ht);
	ENTRY query, *ret;
	// copy goals into hash table
	for (size_t i = 0; i < ngoals; i++) {
		query.key = goals[i]->name;
		query.data = goals[i];
		ASARR_INSERT(finalized, goals[i]);
		PEONZ(hsearch_r, query, ENTER, &ret, &ht);
	}
	query = (ENTRY) { 0 };
	// find each goals' dependency in the list or not
	for (size_t i = 0; i < ngoals; i++) {
		PGoal_t goal = goals[i];
		char **depname = goal->depname;
		goal->dep = PEONZ(malloc, goal->ndep * sizeof(PGoal_t));
		for (size_t j = 0; j < goal->ndep; j++) {
			char *curdep = query.key = depname[j];
			PGoal_t gresolved = NULL;
			// check if goal is laready listed
			if (hsearch_r(query, FIND, &ret, &ht) != 0) {
				gresolved = (PGoal_t) ret->data;
			}
			// goal not in list, determine if it is a file
			else if (errno == ESRCH) {
				// check if file exists
				if (access(curdep, F_OK) != -1) {
					gresolved = gcreate(PEONZ(strdup, curdep), NULL, NULL, 0, NULL, 0);
					ASARR_INSERT(finalized, gresolved);
				}
				else {
					fprintf(stderr, "Error: Dependency \"%s\" of goal \"%s\" is missing.\n",
						depname[j], goal->name);
					return -1;
				}
			}
			else {
				fprintf(stderr, "Fatal: hsearch() mem error\n");
				return -1;
			}
			goal->dep[j] = gresolved;
			free(curdep);
		}
		free(depname);
	}
	hdestroy_r(&ht);
	free(goals);
	// now the list includes resovled file-goals
	*goallist = ASARR_GET(finalized);
	*ng = ASARR_SIZE(finalized);
	return 0;
}

