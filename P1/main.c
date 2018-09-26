/* author1: Dasong Gao
 * author2: Haozhe Luo
 */
#include "utils.h"

int main(int argc, char *argv[]) {
	ps_ops options;
	int n_proc = 0;
	//proc_info *pis;
	if (parse_ops(argc, argv, &options) != 0) {
		fprintf(stderr, "Invalid ps options, exiting.\n");
		exit(-1);
	}
	pid_t *pids = NULL;
	list_pids(NULL, &n_proc);
	if ((pids = calloc(n_proc, sizeof(pid_t))) == NULL) {
		fprintf(stderr, "calloc failed");
		exit(-1);
	}
	n_proc = list_pids(pids, &n_proc);

	print_proc_infos(&options, pids, n_proc);
	free(pids);
	return 0;
	/*read_proc_infos(NULL, &n_proc);
	if ((pis = calloc(n_proc, sizeof(proc_info))) == NULL) {
		fprintf(stderr, "calloc failed");
		exit(-1);
	}
	read_proc_infos(pis, &n_proc);
	output_proc_info(&options, pis, n_proc);
	free(pis);
	return 0;*/
}

