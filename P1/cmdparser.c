#include "cmdparser.h"

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

//it's trivial
int read_proc_info(pid_t pid, proc_info *proc_info) {
	// path buffer
	char buf[32];
	snprintf(buf, sizeof(buf), "/proc/%d/stat", pid);
	FILE* stat_file = fopen(buf, "r");
	fscanf(stat_file, "%*d (%s) %c", &proc_info->cmd, &proc_info->state, &?);

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

