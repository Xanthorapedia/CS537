#include "parser.h"
#include "autoarr.h"
#include "utils.h"

// A macro for fetching the n'the group from mathes
// Basically strdup's from str + start_offset to str + end_offset
#define REGEX_GETGROUP(str, matches, n) \
		PEONZ(strndup, \
			(str) + (matches)[n].rm_so, \
			(matches)[n].rm_eo - (matches)[n].rm_so \
		)

// the lower and upper bounds of the max number of regex matches to consider
const int REGEX_NGROUPS = 5;
const int INIT_MAXMATCH = 16;
const int FINAL_MAXMATCH = 256;

// regex patterns for matching target and command line
const char TGT_PTN[] = "^([^[:space:]:]+)\\s*:((\\s*[^[:space:]:]+)*)\\s*";
const char CMD_PTN[] = "^\t([^<>]*)\\s*(<.+)?(>.+)?\\s*";

// resolves goal references and print error if not found
int mresovle(ASArr *pgoallist, HTable *ht);

// Splits "str," delimited by "delim" into "nparts"
char **splits(char *str, char *delim, int *nparts);

// Try to search for the goal named name from the goal table
// Returns NULL on failure
PGoal_t forname(char *name, HTable *goaltable);

int mparse(char *mpath, ASArr *pgoallist) {
	// compile regex
	// uses POSIX Extended Regular Expression syntax (group without "\(")
	// and enable '^' and '$'
	regex_t tgt_reg, cmd_reg;
	PEONN(regcomp, &tgt_reg, TGT_PTN, REG_EXTENDED | REG_NEWLINE);
	PEONN(regcomp, &cmd_reg, CMD_PTN, REG_EXTENDED | REG_NEWLINE);
	regmatch_t matches[REGEX_NGROUPS + 1];
	// for creating htable later
	int nobjs = 0;
	// line #
	int lineno = 0;
	// goal index
	int idx = 0;

	FILE *mfile  = PEONZ(fopen, mpath, "r");
	if (mfile == NULL) {
		exit(-1);
	}
	char   *line = NULL;
	size_t  len  = 0;
	ssize_t nchar = 0;
	PGoal_t newgoal = NULL;
	PCmd_t  newcmd  = NULL;
	ASARR_DEFTYPE(pgoallist, PGoal_t);
	ASARR_INIT(pcmdlist,  PCmd_t);
	// flag to test for dangling cmd
	int in_goal = 0;
	while ((nchar = getline(&line, &len, mfile)) != -1) {
		lineno++;
		// skip empty lines or lines with only "\n"
		if ((nchar == 1 && line[0] == '\n') || line[0] == '#' || nchar == 0)
			continue;
		// a goal
		if (regexec(&tgt_reg, line, REGEX_NGROUPS, matches, 0) == 0) {
			// parse goal
			char *tgtname = REGEX_GETGROUP(line, matches, 1);
			char *depstr  = REGEX_GETGROUP(line, matches, 2);
			int ndep = 0;
			char **depnames = splits(depstr, " \r\t\f\v", &ndep);
			free(depstr);

			// record new goal and throw the old cmds away
			newgoal = gcreate(tgtname, depnames, NULL, ndep, NULL, 0);
			nobjs += ndep + 1;
			ASARR_RENEW(pcmdlist);
			ASARR_INSERT(pgoallist, newgoal);
			in_goal = 1;
			newgoal->metadata.idx = idx++;
			newgoal->metadata.lineno = lineno;
		}
		// a cmd
		else if (regexec(&cmd_reg, line, REGEX_NGROUPS, matches, 0) == 0) {
			if (!in_goal) {
				fprintf(stderr, "%d: Dangling command line: %s\n", lineno, line);
				goto die;
			}
			// parse cmd
			char *args = REGEX_GETGROUP(line, matches, 1);
			// char *fin  = REGEX_GETGROUP(line, matches, 2);
			// char *fout = REGEX_GETGROUP(line, matches, 3);
			char **argv = splits(args, " \r\t\f\v", NULL);
			free(args);

			// cmd may be empty
			if (argv != NULL) {
				// record new cmd and put into the list of the current goal
				newcmd = ccreate(argv);
				// TODO redirect fin, fout
				ASARR_INSERT(pcmdlist, newcmd);
				newgoal->ncmd = ASARR_SIZE(pcmdlist);
				newgoal->cmd  = ASARR_GET(pcmdlist);
			}
		}
		else {
			fprintf(stderr, "%d: Invalid line: %s\n", lineno, line);
			goto die;
		}
	}
	// embark the output
	HTable goaltable = { 0 };
	PEONZ(hcreate_r, nobjs, &goaltable);
	if (mresovle(pgoallist, &goaltable) != 0) {
		fprintf(stderr, "Cannot resolve dependencies.\n");
		return -1;
	}
	hdestroy_r(&goaltable);

	// clean up
	ASARR_STRIP(pcmdlist);
	if (line != NULL)
		free(line);
	regfree(&tgt_reg);
	regfree(&cmd_reg);
	fclose(mfile);
	return 0;

die:
	ASARR_DESTROY(pgoallist);
	ASARR_DESTROY(pcmdlist);
	if (line != NULL)
		free(line);
	regfree(&tgt_reg);
	regfree(&cmd_reg);
	fclose(mfile);
	return -1;
}

/* store all the goals into a hash table, for all the dependencies
   mentioned in the file, find them in hash table. If successfully
   found, replace the name of the goal to a goal struct. If the goal
   cannot be found, print the error message and return -1
   param: goals: goal array
          ngoals: length of goals
 */
int mresovle(ASArr *pgoallist, HTable *ht) {
	// restore the list
	ASARR_DEFTYPE(pgoallist, PGoal_t);
	int ngoals = ASARR_SIZE(pgoallist);
	PGoal_t *goals = ASARR_GET(pgoallist);

	ENTRY query, *ret;
	// copy goals into hash table
	for (int i = 0; i < ngoals; i++) {
		if (forname(goals[i]->name, ht) != NULL) {
			fprintf(stderr, "%d: duplicate target '%s'\n", goals[i]->metadata.lineno, goals[i]->name);
			return -1;
		}
		query.key  = goals[i]->name;
		query.data = goals[i];
		if (hsearch_r(query, ENTER, &ret, ht) != 0 && errno == ENOMEM) {
			fprintf(stderr, "Fatal: hsearch() mem error\n");
			return -1;
		}
	}
	// new goal index
	int idx = ngoals;
	// determine if each goals' dependency is in the list or not
	for (int i = 0; i < ngoals; i++) {
		PGoal_t goal = ASARR_GET(pgoallist)[i];
		char **depname = goal->depname;
		goal->dep = PEONZ(malloc, goal->ndep * sizeof(PGoal_t));
		for (int j = 0; j < goal->ndep; j++) {
			char *curdep = query.key = depname[j];
			PGoal_t gresolved = NULL;
			// goal not in list, determine if it is a file
			if ((gresolved = forname(curdep, ht)) == NULL) {
				// check if file exists
				if (access(curdep, F_OK) != -1) {
					gresolved = gcreate(curdep, NULL, NULL, 0, NULL, 0);
					gresolved->metadata.idx = idx++;
					query.key  = curdep;
					query.data = gresolved;
					if (hsearch_r(query, ENTER, &ret, ht) != 0 && errno == ENOMEM) {
						fprintf(stderr, "Fatal: hsearch() mem error\n");
						return -1;
					}
					ASARR_INSERT(pgoallist, gresolved);
				}
				else {
					fprintf(stderr, "%d: Dependency '%s' of target '%s' is missing.\n",
						goal->metadata.lineno, depname[j], goal->name);
					return -1;
				}
			}
			goal->dep[j] = gresolved;
		}
		free(depname);
	}
	return 0;
}

char **splits(char *str, char *delim, int *nparts) {
	ASARR_INIT(strings, char *);
	char *token = strtok(str, delim);
	while (token != NULL) {
		ASARR_INSERT(strings, strdup(token));
		token = strtok(NULL, delim);
	}
	if (nparts != NULL)
		*nparts = ASARR_SIZE(strings);
	if (ASARR_SIZE(strings) == 0) {
		ASARR_DESTROY(strings);
		return NULL;
	}
	else
		return ASARR_STRIP(strings);
}

PGoal_t forname(char *name, HTable *goaltable) {
	ENTRY *ret, query = { .key = name, .data = NULL };
	if (hsearch_r(query, FIND, &ret, goaltable) != 0) {
		return (PGoal_t) ret->data;
	}
	return NULL;
}

