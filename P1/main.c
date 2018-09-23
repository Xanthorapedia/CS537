#include "utils.h"
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	/*int n_proc;
	list_pids(NULL, &n_proc);
	pid_t *pids = calloc(n_proc, sizeof(pid_t));
	int got = list_pids(pids, &n_proc);
	printf("n_proc: %d, got: %d\n", n_proc, got);*/
	// int n_proc;
	// read_proc_infos(NULL, &n_proc);
 //    proc_info *proc_infos = calloc(n_proc, sizeof(proc_info));
 //    int got = read_proc_infos(proc_infos, &n_proc);
 //    printf("n_proc: %d, got: %d\n", n_proc, got);
 //    for (int i = 0; i < got; i++) {
	// 	//printf("%d ", pids[i]);
 //    }
	ps_ops options;
	int n_proc = 0;
	pid_t *pids;
	proc_info *pi;
    uid_t uid = getuid();
	if (parse_ops(argc, argv, &options) != 0) {
		fprintf(stderr, "parse options failed");
        exit(0);
	}
	list_pids(NULL, &n_proc);
//    printf("n_proc = %d\n", n_proc);
	if ((pids = malloc(n_proc * sizeof(pid_t))) == NULL) {
        perror("malloc failed");
    }
	if ((pi = calloc(n_proc, sizeof(proc_info))) == NULL) {
        perror("calloc failed");
    }
	list_pids(pids, &n_proc);
//    printf("2*list suc\n");
	for (int i = 0; i < n_proc; i++) {
//        printf("%d: %d lalala %lu lalala\n", i, (int)pids[i], pi[i].utime);
        read_proc_info(pids[i], &pi[i]);
//        printf("%d\n", f);
//        printf("%d\n", i);
	}
    for (int i = 0; i < n_proc; i++) {
        if (options.flags & S_PID) {
            if (pi[i].pid == options.pid) {
                output_proc_info(&options, &pi[i]);
            }
        }
        else {
            if (uid == pi[i].uid) {
                output_proc_info(&options, &pi[i]);
            }
        }
    }
//	output_proc_info(&options, pi, n_proc);
}

