#include "parser.h"

int main(int argc, char *argv[]) {
	proc_info pi;
	pi.cmd = malloc(128 * sizeof(char));
	read_proc_info(atoi(argv[1]), &pi);
	printf("%d (%s) %c %d %d %d\n", pi.pid, pi.cmd, pi.state);
	free(pi.cmd);
}