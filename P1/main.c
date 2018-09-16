#include "utils.h"

int main(int argc, char *argv[]) {
	/*int n_proc;
	list_pids(NULL, &n_proc);
	pid_t *pids = calloc(n_proc, sizeof(pid_t));
	int got = list_pids(pids, &n_proc);
	printf("n_proc: %d, got: %d\n", n_proc, got);*/
	int n_proc;
	ps_ops ops;
	ops.flags = DEFAULT_FLAGS | STATE | UTIME | MEMSZ | USRID;
	read_proc_infos(NULL, &n_proc);
    proc_info *proc_infos = calloc(n_proc, sizeof(proc_info));
    int got = read_proc_infos(proc_infos, &n_proc);
    printf("n_proc: %d, got: %d\n", n_proc, got);
    for (int i = 0; i < got; i++) {
		//printf("%d ", pids[i]);
    }
	output_proc_info (&ops, proc_infos, n_proc);
}

