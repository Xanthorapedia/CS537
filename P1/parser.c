#include "parser.h"

// Parses cmd args and stores results in "options"
// Returns 0 on success, other on false
int parse_ops(int argc, char *argv[], ps_ops *options) {
	int opt;
	if (!options) {
		return -1;
	}
	options->flags = 0;
	if (argc == 1) {
		options->flags = DEFAULT_FLAGS;
	}
	while ((opt = getopt(argc, argv, "p:sUSvc")) != -1) {
		switch (opt) {
            case 'p':
                options->flags = options->flags | S_PID;
                options->pid = atoi(optarg);
                printf("p flag success\n");
                break;
            case 's':
                options->flags = options->flags | STATE;
                printf("s flag success\n");
                break;
            case 'U':
            	options->flags = options->flags | UTIME;
            	printf("U flag success\n");
                break;
            case 'S':
            	options->flags = options->flags | STIME;
            	printf("S flag success\n");
                break;
            case 'v':
            	options->flags = options->flags | MEMSZ;
            	printf("v flag success\n");
                break;
            case 'c':
            	options->flags = options->flags | CMDLN;
            	printf("c flag success\n");
                break;
            default:
                fprintf(stderr, "Usage: %s invalid argument", argv[0]);
                exit(0);
        }
	}
	return 0;
}

// Lists all pids currently in /proc
// If "pids" is NULL, the function only updates "n_proc" to the number of
// process present
// Else, at most "*n_proc" process infos are retrieved to "pids"
// Returns the actuall number of pids fetched
int list_pids(pid_t *pids, int *n_proc) {
	DIR *dir;
	struct dirent *entry;
	*n_proc = 0;
	if ((dir = opendir("/proc")) == NULL) {
		fprintf(stderr, "Usage: /proc cannot be opened");
		return -1;
	}
	else {
		errno = 0;
		while (1) {
			if ((entry = readdir(dir)) != NULL){
				if (pids == NULL) {
					*n_proc++;
				}
				else {
					pids + n_proc = atoi(entry->d_name);
					*n_proc++;
				}
			}
			else {
				if (errno != 0) {
					continue;
				}
				else {
					break;
				}
			}
		}
		closedir(dir);
	}
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


// take flags and process information, output information
// according to the flags. output all processes or specific
// one will be decided in main
int output_proc_info (ps_ops *options, proc_info *pi, int n_proc) {
	for (int i = 0; i < n_proc; i++) {
		// output buffer
		// ?????????????????don't know what should be the buffer size??????????????????
		char buffer[256];
		// counter for sprintf
		int c;
		c = sprintf(buffer, "%i:", (pi + i)->pid);
		if (options->flags & STATE) {
			c += sprintf(buffer + c, " %c", (pi + i)->state);
		}
		if (options->flags & UTIME) {
			c += sprintf(buffer + c, " utime=%lu", (pi + i)->utime);
		}
		if (options->flags & STIME) {
			c += sprintf(buffer + c, " stime=%lu", (pi + i)->stime);
		}
		if (options->flags & MEMSZ) {
			c += sprintf(buffer + c, " size=%lu", (pi + i)->vmsize);
		}
		if (options->flags & CMDLN) {
			c += sprintf(buffer + c, " [%s]", (pi + i)->cmd);
		}
		printf("%s\n", buffer);
	}
}

