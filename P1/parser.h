#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// flags
#define S_PID 1		// -p (specific pid)
#define STATE 2		// -s (program state)
#define UTIME 4		// -U (user time)
#define STIME 8		// -S (system time)
#define MEMSZ 16	// -v (virtual memory size)
#define CMDLN 32	// -c (comand line)

#define DEFAULT_FLAGS (UTIME | CMDLN)

// parsed cmd options
typedef struct {
	int   flags;
	pid_t pid;
} ps_ops;

// parsed process info
typedef struct {
	char state;
	pid_t pid;
	char *cmd;
	unsigned long utime;
	unsigned long stime;
	unsigned long vmsize;
} proc_info;

int parse_ops(int argc, char *argv[], ps_ops *options);

int read_proc_info(pid_t pid, proc_info *pi);
