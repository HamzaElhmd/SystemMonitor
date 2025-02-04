#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include "../include/Cpu.h"
#include "../include/Process.h"
#include "../include/Mem.h"


void displayCpuStatistics(const CpuCollection cpus);

void displayMemStatistics(const MemStat memInfo);

void displayProcesses(const ProcessCollection processes);

int main (int argc, char *argv[]) {

	printf("::::::: Xen System Monitor :::::::\n");
	int nProcessors, err;
	CpuCollection *cpus;
	MemStat memInfo;
	ProcessCollection *processes;

	nProcessors = getNProcessors();
	if (nProcessors == -1) {
		fprintf(stderr, "ERROR : An error has occured. Aborting...\n");
		return -1;
	}

	cpus = createCpuCollection(nProcessors);
	if (!cpus) {
		fprintf(stderr, "ERROR : An error has occured. Aborting...\n");
		return -1;
	}

	err = parseCpuStatCollection(cpus);
	if (err == -1) {
		fprintf(stderr, "ERROR : An error has occured. Aborting...\n");
		return -1;
	}

	err = parseMemStat(&memInfo);
	if (err == -1) {
		fprintf(stderr, "An error occured. Aborting...\n");
		return 1;
	}

	processes = createProcessCollection();
	if (!processes) {
		fprintf(stderr, "ERROR : An error has occured. Aborting...\n");
		return -1;
	}

	err = parseProcessCollection(processes);
	if (err == -1) {
		fprintf(stderr, "An error occured. Aborting...\n");
		return 1;
	}

	displayCpuStatistics(*cpus);
	displayMemStatistics(memInfo);
	displayProcesses(*processes);

	destroyProcessCollection(processes);
	destroyCpuCollection(cpus);
	return 0;
}


void displayCpuStatistics(const CpuCollection cpus) {

	if (cpus.processors == NULL || cpus.size < 1) {
		fprintf(stderr, "ERROR : Can't print empty CPU list.\n");
		return;
	}

	double usagePercentage = 0;
	fprintf(stdout, "==========================\n");
	for (int i = 0; i < cpus.size; i++) {
		usagePercentage = calculateUsagePercentage(cpus.processors[i]);
	    	fprintf(stdout, "\033[%d;1H", 3 + i); 	
		fprintf(stdout, "CPU %d:\t%lf %%\t||\n",
				cpus.processors[i].processorId,
				usagePercentage);
	}
	fprintf(stdout, "==========================\n");

}

void displayMemStatistics(const MemStat memInfo) {

	int rightOffset = strlen("==========================\n");
	double ramUsage, swapUsage, ramTotal, swapTotal;
	
	ramUsage = memInfo.MemTotal - memInfo.MemFree - memInfo.Cached;
	swapUsage = memInfo.SwapTotal - memInfo.SwapFree;

	ramUsage /= pow(10.0, 6);
	swapUsage /= pow(10.0, 6);
	ramTotal = memInfo.MemTotal / pow(10.0, 6);
	swapTotal = memInfo.SwapTotal / pow(10.0, 6);

	fprintf(stdout, "\033[10;35H");
	fprintf(stdout, "RAM: %.2lf GB  /  %.2lf GB\n", ramUsage, ramTotal);
	fprintf(stdout, "\033[11;35H");
	fprintf(stdout, "Swap: %.2lf GB  /  %.2lf GB\n", swapUsage, swapTotal);

}

void displayProcesses(const ProcessCollection processes) {


}
