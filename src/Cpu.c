#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <inttypes.h>
#include "../include/Cpu.h"

#define CPUINFO_SIZE 20000

static int isLineCpu (const char *line) {

	regex_t cpuRgx;
	char *cpuPattern = "^processor";

	if (regcomp(&cpuRgx, cpuPattern, REG_EXTENDED) != 0) {
		fprintf(stderr, "ERROR : Failed to compile regex %s.\n", cpuPattern);
		return -1;
	}

	if (regexec(&cpuRgx, line, 0, NULL, 0) != REG_NOMATCH) {
		regfree(&cpuRgx);
		return 1;
	}

	regfree(&cpuRgx);
	return 0;
}

/* Get the number of logical cores of the system (Processor IDs) */
/** Returns the an amount in size_t **/
int getNProcessors () {

	size_t nProcessors = 0;
    	FILE *cpuInfoFile;
	char *cpuInfoContent, *saveptr, *line;
	char *cpuInfoPath = "/proc/cpuinfo";

	cpuInfoFile = fopen(cpuInfoPath, "r"); 
    	if (!cpuInfoFile) {
        	fprintf(stderr, "ERROR: Failed to open %s. File not found or permissions denied.\n", cpuInfoPath);
        	return -1;
    	}

	cpuInfoContent = (char*) malloc(CPUINFO_SIZE);
	if (!cpuInfoContent) {
		fprintf(stderr, "ERROR : Failed to allocate memory for the content of %s.\n", cpuInfoPath);
		fclose(cpuInfoFile);
		return -1;
	}

	fread(cpuInfoContent, CPUINFO_SIZE, 1, cpuInfoFile);

	line = strtok_r(cpuInfoContent, "\n", &saveptr);

	while (line != NULL) {
		if (isLineCpu(line) == 1) {
			nProcessors++;
		}
		line = strtok_r(NULL, "\n", &saveptr);
	}

	free(cpuInfoContent);
    	fclose(cpuInfoFile);
    	return nProcessors;

}

/* Create CPU statistics collection of all cores */
CpuCollection* createCpuCollection (int nProcessors) {

	CpuCollection *cpus;

	/* Number of CPU cores must be equal to or larger than 1 */
	if (nProcessors < 1) {
		fprintf(stderr, "ERROR : Failed to create cpu stats collection : Number of cores => %d.\n", nProcessors);
		return NULL;
	}

	cpus = (CpuCollection*) malloc(sizeof(CpuCollection));
	if (!cpus) {
		fprintf(stderr, "ERROR : Failed to allocate memory for a cpu collection.\n");
		return NULL;
	}
	cpus->size = nProcessors;

	cpus->processors = (Cpu*) malloc(sizeof(Cpu) * nProcessors);
	if (!cpus->processors) {
		fprintf(stderr, "ERROR : Failed to allocate memory for array of processor cores.\n");
		free(cpus);
		return NULL;
	}

	return cpus;
}


/* Parse each line of /resources/cpu.txt into a CpuStat structure */
int parseCpuStatCollection(CpuCollection *cpus) {

	FILE *cpuStatFile;
	char *cpuStatPath = "/proc/stat", *line = NULL;
	size_t len = 0, read;
	int i = 0;

	if (!cpus) {
		fprintf(stderr, "ERROR : Failed to parse cpu statistics. Invalid arguments.\n");
		return -1;
	}

	cpuStatFile = fopen(cpuStatPath, "r");
	if (!cpuStatFile) {
		fprintf(stderr, "ERROR : Failed to open file %s. File not found or permissions denied.\n", cpuStatPath);
		return -1;
	}

	read = getline(&line, &len, cpuStatFile);
	if (read == -1) {
		fprintf(stderr, "ERROR : Failed to read from %s.\n", cpuStatPath);
		free(line);
		fclose(cpuStatFile);
		return -1;
	}

	while (getline(&line, &len, cpuStatFile) != -1 && i < cpus->size) {
		cpus->processors[i].processorId = i;
		sscanf(line, "%*s %"PRIu32 "%*u %"PRIu32 "%"PRIu32 "%"PRIu32,
				&cpus->processors[i].userTime,
				&cpus->processors[i].sysTime,
				&cpus->processors[i].idleTime,
				&cpus->processors[i].ioWaitTime);
		i++;

	}

	free(line);
	fclose(cpuStatFile);
	 return 0;
}

void printCpuCollection(const CpuCollection *cpus) {

	for (int i = 0; i < cpus->size; i++) {
		fprintf(stdout, "===============================\n");
		fprintf(stdout, "Cpu %d | ", cpus->processors[i].processorId);
		fprintf(stdout, "%"PRIu32"\t", cpus->processors[i].userTime);
		fprintf(stdout, "%"PRIu32"\t", cpus->processors[i].sysTime);
		fprintf(stdout, "%"PRIu32"\t", cpus->processors[i].idleTime);
		fprintf(stdout, "%"PRIu32"\n", cpus->processors[i].ioWaitTime);
	}

}


/** Free the memory of CPU statistics collection **/
void destroyCpuCollection(CpuCollection *cpus) {

	if (cpus != NULL) {
		free(cpus->processors);
		free(cpus);
	}
	

}

/* Calculates CPU usage percentage */
/* return a percentage value between 0.00 and 100.00 on success and -1.00 on error */
double calculateUsagePercentage(Cpu cpuStat) {

	double percentage, usageTime, idleTime, totalTime;

	usageTime = (cpuStat.userTime + cpuStat.sysTime);

	idleTime = (cpuStat.idleTime + cpuStat.ioWaitTime);
	
	totalTime = (usageTime + idleTime);

	percentage = (usageTime / totalTime) * 100;

	return percentage;

}


