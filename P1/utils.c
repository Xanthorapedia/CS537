#include "utils.h"

FILE *open_pid_file_safe(pid_t pid, char *fn);

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
	DIR *proc_dir;
	struct dirent *process_dir;
	*n_proc = pids == NULL ? INT_MAX : *n_proc;
	// check for access problems
	if (access("/proc", R_OK) != 0 || (proc_dir = opendir("/proc")) == NULL) {
		fprintf(stderr, "Error: /proc cannot be opened");
		return -1;
	}

	// fill in valid pid directories
	int i;
	for (i = 0; i < *n_proc && (process_dir = readdir(proc_dir)) != NULL; i++) {
		char *pid_str = process_dir->d_name;
		// if is not number (valid pid), skip and don't increment counter
		if (pid_str[0] < '0' || pid_str[0] > '9') {
			i--;
			continue;
		}
		// if not required to provide info, skip
		else if (pids == NULL)
			continue;
		pids[i] = (pid_t) strtol(pid_str, NULL, 10);
		// printf("%d ", pids[i]);
	}
	closedir(proc_dir);
	if (pids == NULL) {
		*n_proc = i;
		return 0;
	}
	return i;
}

// Reads the process info of [pid]
int read_proc_info(pid_t pid, proc_info *pi) {
	// path buffer
	char path[PATH_SIZE];
	FILE* p_file = NULL;

	// stat file
	if ((p_file = open_pid_file_safe(pid, "stat")) == NULL)
		return (pi->pid = -1);
	fscanf(p_file, "%d %s %c", &pi->pid, pi->cmd, &pi->state);
	// get rid of the surrouding brackets
	char *c = pi->cmd;
	assert(c != NULL);
	while ((*c = *(c + 1)) != '\0')
		c++;
	*(c - 1) = '\0';
	// skip unwanted info
	for (int i = 0; i < STAT_SKIP_TOKEN / 2; i++)
		fscanf(p_file, "%*d %*d");
	fscanf(p_file, "%lu %lu", &pi->utime, &pi->stime);
	fclose(p_file);

	// now it's statm
	if ((p_file = open_pid_file_safe(pid, "statm")) == NULL)
		return (pi->pid = -1);
	fscanf(p_file, "%lu", &pi->vmsize);
	fclose(p_file);

	// now it's status
	if ((p_file = open_pid_file_safe(pid, "status")) == NULL)
		return (pi->pid = -1);
	for (int i = 0; i < STATUS_SKIP_LINE; i++)
		fscanf(p_file, "%*[^\n]\n");
	fscanf(p_file, "Uid: %d", &pi->uid);
	fclose(p_file);
	return 0;
}

// safely opens a pid property file "fn" of "pid"
FILE *open_pid_file_safe(pid_t pid, char *fn) {
	char path[PATH_SIZE];
	snprintf(path, sizeof(path), PATH_HEAD"/%d/", pid);
	strncat(path, fn, sizeof(path) - strlen(path));
	if (access(path, R_OK) != 0)
		return NULL;
	FILE *p_file = fopen(path, "r");
	assert(p_file != NULL);
	return p_file;
}

// Retrieves an array of process info based on "options"
// If "proc_infos" is NULL, the function only updates "n_proc" to the number of
// process present
// Else, at most "*n_proc" process infos are retrieved to "proc_infos"
// Returns the actuall number of process information fetched
int read_proc_infos(proc_info *proc_infos, int *n_proc) {
	if (proc_infos == NULL) {
		list_pids(NULL, n_proc);
		return 0;
	}

	pid_t *pids = malloc(*n_proc * sizeof(pid_t));
	int n_got = list_pids(pids, n_proc);

	for (int i = 0; i < n_got; i++)
		read_proc_info(pids[i], &proc_infos[i]);

	return n_got;
}


// int n_proc;
// proc_info **proc_infos = NULL;
// // 1
// parse_proc(options, NULL, n_proc);
// proc_infos = malloc(sizeof(proc_info*) * n_proc);
// // 2
// parse_proc(options, proc_infos, n_proc);


// Take flags and process information, output information
// according to the flags. Output all processes or specific
// one will be decided in main
int output_proc_info (ps_ops *options, proc_info *pi, int n_proc) {
	for (int i = 0; i < n_proc; i++) {
		printf("%d:", (pi + i)->pid);
		if (options->flags & STATE) {
			printf(" %c", (pi + i)->state);
		}
		if (options->flags & USRID) {
			printf(" uid=%d", (pi + i)->uid);
		}
		if (options->flags & UTIME) {
			printf(" utime=%lu", (pi + i)->utime);
		}
		if (options->flags & STIME) {
			printf(" stime=%lu", (pi + i)->stime);
		}
		if (options->flags & MEMSZ) {
			printf(" size=%lu", (pi + i)->vmsize);
		}
		if (options->flags & CMDLN) {
			printf(" [%s]", (pi + i)->cmd);
		}
		printf("\n");
	}
}

