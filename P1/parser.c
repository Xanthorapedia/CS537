#include "parser.h"

// Parses cmd args and stores results in "options"
// Returns 0 on success, other on false
int parse_ops(int argc, char *argv[], ps_ops *options) {
	if (!options)
		return -1;
	options->flags = DEFAULT_FLAGS;

	// parse
	// if ()
	// 	options->flags |= PROG_STATE
}

// Lists all pids currently in /proc
// If "pids" is NULL, the function only updates "n_proc" to the number of
// process present
// Else, at most "*n_proc" process infos are retrieved to "pids"
// Returns the actuall number of pids fetched
int list_pids(pid_t pids, int *n_proc) {
	return 0;
}

// Reads the process info of [pid]
int read_proc_info(pid_t pid, proc_info *pi) {
	// path buffer
	char path[32];
	FILE* afile = NULL;

	snprintf(path, sizeof(path), "/proc/%d/stat", pid);
	if (access(path, R_OK) != 0)
		return (pi->pid = -1);
	afile = fopen(path, "r");
	assert(afile != NULL);
	fscanf(afile, "%d %s %c", &pi->pid, pi->cmd, &pi->state);
	char *c = pi->cmd;
	while ((*c = *(c + 1)) != '\0')
		c++;
	*(c - 1) = '\0';
	// skip unwanted info
	for (int i = 0; i < 5; i++)
		fscanf(afile, "%*d %*d");
	fscanf(afile, "%lu %lu", &pi->utime, &pi->stime);
	fclose(afile);

	// now it's statm
	strcat(path, "m");
	if (access(path, R_OK) != 0)
		return (pi->pid = -1);
	afile = fopen(path, "r");
	assert(afile != NULL);
	fscanf(afile, "%lu", &pi->vmsize);
	fclose(afile);

	return 0;
}

// Retrieves an array of process info based on "options"
// If "proc_infos" is NULL, the function only updates "n_proc" to the number of
// process present
// Else, at most "*n_proc" process infos are retrieved to "proc_infos"
// Returns the actuall number of process information fetched
int parse_proc(ps_ops *options, proc_info *proc_infos[], int *n_proc) {
	if (!proc_infos) {
		// *n_proc = get_n_process();
		return 0;
	}

}


// int n_proc;
// proc_info **proc_infos = NULL;
// // 1
// parse_proc(options, NULL, n_proc);
// proc_infos = malloc(sizeof(proc_info*) * n_proc);
// // 2
// parse_proc(options, proc_infos, n_proc);

