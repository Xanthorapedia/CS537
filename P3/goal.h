/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "stdinclude.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

#include "autoarr.h"

#pragma once

// A command line object that can be easily executed by exec
typedef struct __cmd {
	char **argv;
	// input and output file name
	char *ifn;
	char *ofn;
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
	int ndep;
	// list of command lines to execute
	struct __cmd  **cmd;
	int ncmd;
	// metadata used by the functions
	// basically the index in the list (file) and the time of modification
	struct _metadata {
		int lineno;
		int idx;
		time_t modt;
	} metadata;
} Goal_t, *PGoal_t;

PCmd_t ccreate(char **argv, char *ifn, char *ofn);
void cdestroy(PCmd_t cmd);
void cprint(PCmd_t cmd);

PGoal_t gcreate(char *name, char **depname, PGoal_t *dep, int ndep, PCmd_t *cmd, size_t ncmd);
void gdestroy(PGoal_t goal);
void gprint(PGoal_t goal);

int mcheckupdate(char *appointed, ASArr *goals, ASArr *updatelist);

int gupdt(PGoal_t goal);

