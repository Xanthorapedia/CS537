#include "stdinclude.h"
#include <errno.h>
#include <search.h>

#pragma once

// A command line object that can be easily executed by exec
typedef struct __cmd {
	size_t argc;
	char **argv;
} Cmd_t, *PCmd_t;

// A 537make goal
typedef struct __goal {
	// name of the goal
	char *name;
	// unresovled dependency names or resolved dependencies
	union {
		char **depname;
		struct __goal **dep;
	};
	size_t ndep;
	// list of command lines to execute
	struct __cmd  **cmd;
	size_t ncmd;
} Goal_t, *PGoal_t;

PCmd_t ccreate(size_t argc, char **argv);
void cdestroy(PCmd_t cmd);
void cprint(PCmd_t cmd);

PGoal_t gcreate(char *name, char **depname, PGoal_t *dep, size_t ndep, PCmd_t *cmd, size_t ncmd);
void gdestroy(PGoal_t goal);
void gprint(PGoal_t goal);

int mresovle(PGoal_t *goallist[], size_t *ng, size_t nbojs);

