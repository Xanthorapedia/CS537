#include "stdinclude.h"
#include <regex.h>
#include <string.h>
#include <sys/types.h>

#include "autoarr.h"
#include "goal.h"

// Parses a makefile into goals
// "mfile" is the path to the makefile
// "mgoals" is set to an array pointing to the goals parsed from the file
// "nmgoals" is set to the number of goals in "mgoals"
// "nbojs" is set to the number of goals + dependencies in the file
// returns 0 on success
int mparse(char *mpath, ASArr *pgoallist);

