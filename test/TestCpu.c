#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../include/Cpu.h"

int main (int argc, char * argv[]) {

	int nProcessors = 0, err;
	CpuCollection *cpus;

	nProcessors = getNProcessors();
	if (nProcessors == -1) {
		fprintf(stderr, "ERROR : An error has occured. Aborting...\n");
		return -1;
	}

	printf("# Processor Cores: %d\n", nProcessors);


	cpus = createCpuCollection(nProcessors);

	err = parseCpuStatCollection(cpus);
	if (err == -1) {
		fprintf(stderr, "ERROR : An error has occured. Aborting...\n");
		return -1;
	}

	printCpuCollection(cpus);

	destroyCpuCollection(cpus);
	return 0;
}
