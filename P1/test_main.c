#include "parser.h"

int main(int argc, char *argv[]) {
	/*proc_info pi;
	ps_ops ops;
	parse_ops(argc, argv, &ops);
	pi.cmd = malloc(128 * sizeof(char));
	read_proc_info(atoi(argv[1]), &pi);
	printf("%d (%s) %c %lu %lu %lu\n", pi.pid, pi.cmd, pi.state, pi.utime, pi.stime, pi.vmsize);
	free(pi.cmd);*/
	int n_proc;
	list_pids(NULL, &n_proc);
	pid_t *pids = calloc(n_proc, sizeof(pid_t));
	int got = list_pids(pids, &n_proc);
	printf("n_proc: %d, got: %d\n", n_proc, got);
	for (int i = 0; i < got; i++) {
		printf("%d ", pids[i]);
	}
}

