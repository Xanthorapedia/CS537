#include "parser.h"

int main(int argc, char *argv[]) {
	proc_info pi;
	pi.cmd = malloc(128 * sizeof(char));
	read_proc_info(atoi(argv[1]), &pi);
	printf("%d (%s) %c %lu %lu %lu\n", pi.pid, pi.cmd, pi.state, pi.utime, pi.stime, pi.vmsize);
	free(pi.cmd);
}

