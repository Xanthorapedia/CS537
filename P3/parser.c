#include "parser.h"
#include "utils.h"

#define REGEX_GETGROUP(str, matches, n) \
		PEONZ(strndup, \
			(str) + (matches)[n].rm_so, \
			(matches)[n].rm_eo - (matches)[n].rm_so \
		)

// the lower and upper bounds of the max number of regex matches to consider
const size_t REGEX_NGROUPS = 5;
const size_t INIT_MAXMATCH = 16;
const size_t FINAL_MAXMATCH = 256;

// regex patterns for matching target and command line
const char TGT_PTN[] = "^([^[:space:]:]+)\\s*:((\\s*\\S+)*)\\s*";
const char CMD_PTN[] = "^\t(\\S+)((\\s*\\S+)*)\\s*(<.+)?(>.+)?\\s*";

// Try to match the regex.
// If all pmatch entries are occupied, *pmatch will double its size and
// matching is performed again. Stops until either full match results are
// obtained or the match size > FINAL_MAXMATCH
int matchall(regex_t *preg, char *str, regmatch_t **pmatch);

// Splits "str," delimited by "delim" into "nparts"
char **splits(char *str, char *delim, int *nparts);

int mparse(char *mpath, PGoal_t *mgoals[], int *nmgoals) {
	// compile regex
	// uses POSIX Extended Regular Expression syntax (group without "\(")
	// and enable '^' and '$'
	regex_t tgt_reg, cmd_reg;
	PEONN(regcomp, &tgt_reg, TGT_PTN, REG_EXTENDED | REG_NEWLINE);
	PEONN(regcomp, &cmd_reg, CMD_PTN, REG_EXTENDED | REG_NEWLINE);
	regmatch_t matches[REGEX_NGROUPS + 1];// = malloc(INIT_MAXMATCH * sizeof(regmatch_t));

	FILE *mfile  = PEONZ(fopen, mpath, "r");
	char   *line = NULL;
	size_t  len  = 0;
	ssize_t nchar = 0;
	PGoal_t newgoal = NULL;
	PCmd_t  newcmd  = NULL;
	ASARR_INIT(pgoallist, PGoal_t);
	ASARR_INIT(pcmdlist,  PCmd_t);
	// flag to test for dangling cmd
	int in_goal = 0;
	while ((nchar = getline(&line, &len, mfile)) != -1) {
		// skip empty lines or lines with only "\n"
		if ((nchar == 1 && line[0] == '\n') || nchar == 0)
			continue;
		// a goal
		if (regexec(&tgt_reg, line, REGEX_NGROUPS, matches, 0) == 0) {
			// parse goal
			char *tgtname = REGEX_GETGROUP(line, matches, 1);
			char *depstr  = REGEX_GETGROUP(line, matches, 2);
			int ndep = 0;
			char **depnames = splits(depstr, " \r\t\f\v", &ndep);
			free(depstr);
			PGoal_t *dep = malloc(ndep * sizeof(PGoal_t));
			for (int i = 0; i < ndep; i++) {
				// the stub goals are temporarily created, should be resolved later
				dep[i] = malloc(sizeof(Goal_t));
				dep[i]->name = depnames[i];
			}
			free(depnames);

			// record new goal and throw the old cmds away
			newgoal = malloc(sizeof(Goal_t));
			newgoal->name = tgtname;
			newgoal->dep = dep;
			newgoal->ndep = ndep;
			ASARR_RENEW(pcmdlist);
			ASARR_INSERT(pgoallist, newgoal);
			in_goal = 1;
		
		}
		// a cmd
		else if (regexec(&cmd_reg, line, REGEX_NGROUPS, matches, 0) == 0) {
			if (!in_goal) {
				// TODO print error
				fprintf(stderr, "Dangling command line.\n");
				goto die;
			}
			// parse cmd
			char *exe  = REGEX_GETGROUP(line, matches, 1);
			char *args = REGEX_GETGROUP(line, matches, 2);
			// char *fin  = REGEX_GETGROUP(line, matches, 4);
			// char *fout = REGEX_GETGROUP(line, matches, 5);
			int nargs = 0;
			char **argv = splits(args, " \r\t\f\v", &nargs);
			free(args);

			// record new cmd and put into the list of the current goal
			newcmd = malloc(sizeof(Cmd_t));
			newcmd->path = exe;
			newcmd->argv = argv;
			// TODO redirect fin, fout
			ASARR_INSERT(pcmdlist, newcmd);
			newgoal->cmd  = ASARR_GET(pcmdlist);
			newgoal->ncmd = ASARR_SIZE(pcmdlist);
		}
		else {
			// TODO error
			//char *colon = strstr(line, ":" == NULL);
			//if (colon == NULL) {
			//	// TODO print error
			//	fprintf(stderr, "No ':' after target name.\n");
			//	goto die;
			//}
		}
	}
	// embark the output
	*mgoals = ASARR_GET(pgoallist);
	*nmgoals = ASARR_SIZE(pgoallist);
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

int matchall(regex_t *preg, char *str, regmatch_t **pmatch) {
	size_t cur_maxmatch = INIT_MAXMATCH;
	int notmatch;
	// enlarge buffer if there is a match but the last pmatch is occupied
	// (possibly unstored results)
	while ((notmatch = regexec(preg, str, cur_maxmatch, *pmatch, 0)) == 0 &&
			*pmatch && (*pmatch)[cur_maxmatch - 1].rm_so != -1 &&
			cur_maxmatch < FINAL_MAXMATCH) {
		cur_maxmatch *= 2;
		*pmatch = PEONZ(realloc, *pmatch, cur_maxmatch);
	}
	return notmatch;
}

char **splits(char *str, char *delim, int *nparts) {
	ASARR_INIT(strings, char *);
	char *token = strtok(str, delim);
	while (token != NULL) {
		ASARR_INSERT(strings, strdup(token));
		token = strtok(NULL, delim);
	}
	*nparts = ASARR_SIZE(strings);
	return ASARR_GET(strings);
}

