#include "stdinclude.h"
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <sys/types.h>

#include "goal.h"

// Parses a makefile into goals
// "mfile" is the path to the makefile
// "mgoals" is set to an array pointing to the goals parsed from the file
// "nmgoals" is set to the number of goals in "mgoals"
// returns 0 on success
int mparse(char *mfile, PGoal_t *mgoals[], size_t *nmgoals);

