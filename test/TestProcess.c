#include <stdio.h>
#include <sys/types.h>
#include "../include/Process.h"

int main(int argc, char *argv[]) {
	int err;
	ProcessCollection *rProcesses;

	rProcesses = createProcessCollection();
	if (!rProcesses) {
		fprintf(stderr, "ERROR : An error has occured. Aborting...\n");
		return -1;
	}

	err = parseProcessCollection(rProcesses);
	if (err == -1) {
		fprintf(stderr, "ERROR : An error has occured. Aborting...\n");
		return -1;
	}

	printf("Process ID : %u\n", rProcesses->processes[0].processId);
	printf("Process User Time: %u\n", rProcesses->processes[0].userTime);
	printf("Process Kernel Time: %u\n", rProcesses->processes[0].sysTime);
	printf("Process Wait Time: %u\n", rProcesses->processes[0].waitTime);
	printf("Process Resident Memory : %u.\n", rProcesses->processes[0].resMem);
	printf("Process Command : %s", rProcesses->processes[0].command);

	destroyProcessCollection(rProcesses);
	return 0;
}
