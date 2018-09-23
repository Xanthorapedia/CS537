#include "utils.h"
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	ps_ops options;
	int n_proc = 0;
	proc_info *pis;
	read_proc_infos(NULL, &n_proc);
	if ((pis = calloc(n_proc, sizeof(proc_info))) == NULL) {
		perror("calloc failed");
	}
	read_proc_infos(pis, &n_proc);
	output_proc_info(&options, pis, n_proc);
	free(pis);
}

