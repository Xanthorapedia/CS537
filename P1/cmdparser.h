#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

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
	pid_t pid;
	char state;
	unsigned long utime;
	unsigned long stime;
	unsigned long vmsize;
	char *cmd;
} proc_info;

int parse_ops(int argc, char *argv[], ps_ops *options);
