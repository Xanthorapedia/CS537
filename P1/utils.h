#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// buffer sizes
#define CMD_SIZE 256	// the size of process' cmd property
#define PATH_SIZE 32	// the size of /proc file path string
#define PATH_HEAD "/proc/"

// flags
#define S_PID 1		// -p (specific pid)
#define STATE 2		// -s (program state)
#define UTIME 4		// -U (user time)
#define STIME 8		// -S (system time)
#define MEMSZ 16	// -v (virtual memory size)
#define CMDLN 32	// -c (comand line)
#define USRID 64	// for debugging
#define DEFAULT_FLAGS (UTIME | CMDLN)

// magic numbers
#define STAT_SKIP_TOKEN 10
#define STATUS_SKIP_LINE 8

// parsed cmd options
typedef struct {
	int   flags;
	pid_t pid;
} ps_ops;

// parsed process info
typedef struct {
	char state;
	pid_t pid;
	uid_t uid;
	unsigned long utime;
	unsigned long stime;
	unsigned long vmsize;
	char cmd[CMD_SIZE];
} proc_info;

int parse_ops(int argc, char *argv[], ps_ops *options);

int read_proc_info(pid_t pid, proc_info *pi);

int read_proc_infos(proc_info *proc_infos, int *n_proc);

int list_pids(pid_t *pids, int *n_proc);

int output_proc_info (ps_ops *options, proc_info *pi);
