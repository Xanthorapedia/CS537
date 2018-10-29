#include "goal.h"
#include "stdinclude.h"
#include "utils.h"

/* store all the goals into a hash table, for all the dependencies
   mentioned in the file, find them in hash table. If successfully
   found, replace the name of the goal to a goal struct. If the goal
   cannot be found, print the error message and return -1
   param: goals: goal array
          ngoals: length of goals
 */
int mresovle(PGoal_t *goals, size_t ngoals) {
	// create hash table
    struct hsearch_data ht = (struct hsearch_data) { 0 };
	PEONZ(hcreate_r, ngoals, &ht);
    // copy goals into hash table
	for (size_t i = 0; i < ngoals; i++) {
		ENTRY e, *ret;
		e.key = goals[i]->name;
		e.data = goals[i];
		PEONZ(hsearch_r, e, ENTER, &ret, &ht);
	}
	ENTRY query, *ret;
	query = (ENTRY) { 0 };
    // find each goals' dependency in the list or not
	for (size_t i = 0; i < ngoals; i++) {
        PGoal_t goal = goals[i];
        char **depname = goal->depname;
        goal->dep = PEONZ(malloc, goal->ndep * sizeof(PGoal_t));
		for (size_t j = 0; j < goal->ndep; j++) {
			query.key = depname[j];
			PEONZ(hsearch_r, query, FIND, &ret, &ht);
			if (ret != NULL) {
				goal->dep[j] = (PGoal_t) ret->data;
                free(depname[j]);
			}
			else {
				fprintf(stderr, "No such goal\n");
				return -1;
			}
		}
        free(depname);
	}
	return 0;
}

