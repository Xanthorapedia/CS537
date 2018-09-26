/* author1: Dasong Gao
 * author2: Haozhe Luo
 */
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// buffer sizes
#define CMD_SIZE 512	// the size of process' cmd property
#define PATH_SIZE 32	// the size of /proc file path string
#define PATH_HEAD "/proc/"

// magic numbers
#define STAT_SKIP_TOKEN 10
#define STATUS_SKIP_LINE 8

// flags
#define S_PID 1		// -p (specific pid)
#define STATE 2		// -s (program state)
#define UTIME 4		// -U (user time)
#define STIME 8		// -S (system time)
#define MEMSZ 16	// -v (virtual memory size)
#define CMDLN 32	// -c (comand line)
#define USRID 64	// print uid (for dbg only)
#define UONLY 128	// current user only (for dbg only)
#define DEFAULT_FLAGS (UTIME | CMDLN | UONLY)

// flag operations
#define OPSETF 1
#define OPCLRF 0
#define SETF(var, flag) ((var) |= (flag))
#define CLRF(var, flag) ((var) &= (~flag))
#define TSTF(var, flag) ((var) & (flag))
#define CLETF(var, flag, set_or_clear) ((set_or_clear) ?\
		SETF((var), (flag)) : CLRF((var), (flag)))

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
	char *cmd;
} proc_info;

int parse_ops(int argc, char *argv[], ps_ops *options);

int read_proc_info(pid_t pid, proc_info *pi);

int list_pids(pid_t *pids, int *n_proc);

int print_proc_info(ps_ops *options, proc_info *pi);

int print_proc_infos(ps_ops *options, pid_t *pids, int n_proc);

