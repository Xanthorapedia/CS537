/* author1: Dasong Gao
 * author2: Haozhe Luo
 */

#include "goal.h"
#include "autoarr.h"
#include "utils.h"

// Get the time of modification of the goal if it is an existent file
time_t getmodtime(char *pathname);

// The recursive utility function that checks for loops and marks the goals
// to be updated. Only update if one of the dependency of the goal is more
// recent and the goal is depended by the appointed goal.
time_t chkuputil(PGoal_t goal, int *visited, int *onstack, int appointed, int headidx, ASArr *updatelist);

int cexec(PCmd_t cmd);

PCmd_t ccreate(char **argv, char *ifn, char *ofn) {
	PCmd_t newcmd = PEONZ(malloc, sizeof(Cmd_t));
	newcmd->argv = argv;
	newcmd->ifn = ifn;
	newcmd->ofn = ofn;
	return newcmd;
}

void cdestroy(PCmd_t cmd) {
	for (char **parg = cmd->argv; *parg != NULL; parg++)
		free(*parg);
	free(cmd->argv);
	free(cmd->ifn);
	free(cmd->ofn);
	free(cmd);
}

PGoal_t gcreate(char *name, char **depname, PGoal_t *dep, int ndep, PCmd_t *cmd, size_t ncmd) {
	PGoal_t newgoal = PEONZ(malloc, sizeof(Goal_t));
	newgoal->name = name;
	if (depname != NULL)
		newgoal->depname = depname;
	else
		newgoal->dep = dep;
	newgoal->ndep = ndep;
	newgoal->cmd = cmd;
	newgoal->ncmd = ncmd;
	newgoal->metadata.modt = getmodtime(name);
	return newgoal;
}

void gdestroy(PGoal_t goal) {
	free(goal->name);
	free(goal->depname);
	for (int i = 0; i < goal->ncmd; i++)
		cdestroy(goal->cmd[i]);
	free(goal->cmd);
	free(goal);
}


void cprint(PCmd_t cmd) {
	printf("  Cmmd @ %p:", cmd);
	for (char **parg = cmd->argv; *parg != NULL; parg++) {
		printf(" \"%s\"", *parg);
	}
	printf("\n");
}

void gprint(PGoal_t goal) {
	printf("Goal %d @ %p:\n", goal->metadata.idx, goal);
	printf("name: %s\n", goal->name);
	printf("%d dependencies:\n", goal->ndep);
	for (int i = 0; i < goal->ndep; i++)
		printf("  Goal %d @ %p: \"%s\"\n", goal->dep[i]->metadata.idx, goal->dep[i], goal->dep[i]->name);
	printf("%d comamnds:\n", goal->ncmd);
	for (int i = 0; i < goal->ncmd; i++) {
		cprint(goal->cmd[i]);
	}
	printf("metadata:\n");
	printf("  idx: %d\n  modtime: %lu\n", goal->metadata.idx, goal->metadata.modt);
	printf("\n");
}

int mcheckupdate(char *appointed, ASArr *goals, ASArr *updatelist) {
	ASARR_DEFTYPE(goals, PGoal_t);
	PGoal_t *goalsarr = ASARR_GET(goals);
	int ng = ASARR_SIZE(goals);
	// default goal
	appointed = appointed == NULL && ng > 0 ? goalsarr[0]->name : appointed;

	// DFS to find loops and goals to be updated
	int *visited = PEONZ(calloc, ng, sizeof(int));
	int *onstack = PEONZ(calloc, ng, sizeof(int));
	int appfound = 0;
	for (int i = 0; i < ng; i++) {
		PGoal_t goal = goalsarr[i];
		int app = 0;
		if (strcmp(goal->name, appointed) == 0)
			app = appfound = 1;
		// +1 to avoid skipping unvisited goal[0]
		if (chkuputil(goal, visited, onstack, app, i + 1, updatelist) < 0) {
			fprintf(stderr, "Bad dependency graph. Stop.\n");
			return -1;
		}
	}
	free(visited);
	free(onstack);
	if (ng > 0 && !appfound) {
		fprintf(stderr, "537make: *** No rule to make target '%s'.  Stop.\n", appointed);
		return -1;
	}
	return 0;
}

time_t chkuputil(PGoal_t goal, int *visited, int *onstack, int appointed, int headidx, ASArr *updatelist) {
	int id = goal->metadata.idx;
	// if was from the same goal, must be circular
	if (onstack[id]) {
		fprintf(stderr, "537make: Circular dependency for target '%s'\n", goal->name);
		return -1;
	}
	if (visited[id] == headidx)
		return 0;
	onstack[id] = visited[id] = 1;

	// good to check
	ASARR_DEFTYPE(updatelist, PGoal_t);
	// when was the latest of us born
	time_t latestbd = goal->metadata.modt;
	for (int i = 0; i < goal->ndep; i++) {
		time_t depbd = chkuputil(goal->dep[i], visited, onstack, appointed, headidx, updatelist);
		if (depbd < 0)
			return -1;
		latestbd = depbd > latestbd ? depbd : latestbd;
	}

	// update only if the latest dep is more recent
	// and the current goal depended on by an appointed goal
	// if no dependecy, must update
	if (appointed &&
			(latestbd > goal->metadata.modt || goal->metadata.modt == 0)) {
//		printf("%s is in\n", goal->name);
		ASARR_INSERT(updatelist, goal);
		latestbd = time(NULL);
	}

	// pop from stack and return the latest mod time on the path we have seen so far
	onstack[id] = 0;
	return latestbd;
}

time_t getmodtime(char *pathname) {
	if (access(pathname, F_OK) == -1)
		return 0;
	struct stat gstat;
	// path interpreted relative to the current working dir
	if (fstatat(AT_FDCWD, pathname, &gstat, 0) != 0) {
		fprintf(stderr, "Fatal: hsearch() mem error\n");
		return -1;
	}
	return gstat.st_mtim.tv_sec;
}

int cexec(PCmd_t cmd) {
	pid_t pid;
	int status = 0;
	if ((pid = fork()) < 0) {
		fprintf(stderr, "Fatal: fork() failed.\n");
		return -1;
	}
	else if (pid != 0)
		waitpid(pid, &status, 0);
	else {
		// print the command
		char **parg = cmd->argv;
		for (; *(parg + 1) != NULL; parg++) {
			printf("%s ", *parg);
		}
		printf("%s", *parg);
		// NULL or "" are all treated as nothing
		if (cmd->ifn != NULL && strlen(cmd->ifn) > 0)
			printf(" < %s", cmd->ifn);
		if (cmd->ofn != NULL && strlen(cmd->ofn) > 0)
			printf(" > %s", cmd->ofn);
		printf("\n");
		// print flushed, good to redirect
		if (cmd->ifn != NULL && strlen(cmd->ifn) > 0) {
			int fd = open(cmd->ifn, O_RDONLY);
			if (fd < 0) {
				fprintf(stderr, "537make: Cannot open '%s' for reading.\n", cmd->ifn);
				exit(-1);
			}
			dup2(fd, 0);
		}
		if (cmd->ofn != NULL && strlen(cmd->ofn) > 0) {
			int fd = open(cmd->ofn, O_WRONLY | O_CREAT, S_IRWXU);
			if (fd < 0) {
				fprintf(stderr, "537make: Cannot open '%s' for writing.\n", cmd->ofn);
				exit(-1);
			}
			dup2(fd, 1);
		}
		// execute new command
		if (execvp(cmd->argv[0], cmd->argv)) {
			fprintf(stderr, "Fatal: execvp() failed.\n");
			exit(-1);
		}
	}
	return status;
}

int gupdt(PGoal_t goal) {
	if (goal->ndep == 0 && goal->ncmd == 0) {
		fprintf(stdout, "537make: Nothing to be done for '%s'.", goal->name);
	}
	for (int i = 0; i < goal->ncmd; i++) {
		PCmd_t cmd = goal->cmd[i];
		int ret = cexec(cmd);
		if (ret != 0) {
			fprintf(stderr, "%d: recipe for target '%s' failed.\n", goal->metadata.lineno, goal->name);
			return ret;
		}
	}
	return 0;
}
