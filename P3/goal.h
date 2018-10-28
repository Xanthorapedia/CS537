#include "stdinclude.h"

#pragma once

// A command line object that can be easily executed by exec
typedef struct __cmd {
	char  *path;
	char **argv;
} Cmd_t, *PCmd_t;

// A 537make goal
typedef struct __goal {
	// name of the goal
	char *name;
	// dependencies
	struct __goal **dep;
	size_t ndep;
	// list of command lines to execute
	struct __cmd  **cmd;
	size_t ncmd;
} Goal_t, *PGoal_t;

