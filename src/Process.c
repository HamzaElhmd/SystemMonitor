#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <regex.h>
#include "../include/Process.h"

#define PROC_ENTRIES_MAX 700
#define PROCSTAT_SIZE_MAX 1024
#define PROCSMAP_SIZE_MAX 120000

uint32_t* getProcessIds(size_t *size) {

	if (!size) {
		fprintf(stderr, "ERROR : Failed to allocate memory for the process ids size.\n");
		return NULL;
	}

	uint32_t *processIds = (uint32_t*) malloc(sizeof(uint32_t) * PROC_ENTRIES_MAX);
	if (!processIds) {
		fprintf(stderr, "ERROR : Couldn't allocate memory for process IDs.\n");
		return NULL;
	}

	struct dirent *entry;
	DIR *procDirectory;
	regex_t processIdRgx;
	char *processIdRgxTxt = "^[0-9]+$";

	if (regcomp(&processIdRgx, processIdRgxTxt, REG_EXTENDED) != 0) {
		fprintf(stderr, "ERROR : Failed to compile the regex expression %s \n", processIdRgxTxt);
		free(processIds);
		return NULL;
	}

	procDirectory = opendir("/proc");
	if (!procDirectory) {
		fprintf(stderr, "ERROR : Failed to open the /proc directory. Not found or permissions issue.\n");
		free(processIds);
		regfree(&processIdRgx);
		return NULL;
	}

	*size = 0;
	while ((entry = readdir(procDirectory)) != NULL) {
		if (regexec(&processIdRgx, entry->d_name, 0, NULL, 0) != REG_NOMATCH) {
			if (*size >= PROC_ENTRIES_MAX) {
				processIds = (uint32_t*) realloc(processIds, sizeof(processIds) * (*size + PROC_ENTRIES_MAX));
				if (!processIds) {
					fprintf(stderr, "ERROR : Failed to re-allocate memory for process IDs array.\n");
					free(processIds);
					regfree(&processIdRgx);
					closedir(procDirectory);
					return NULL;
				}
			}
			processIds[(*size)++] = (uint32_t) atoi(entry->d_name);
		}
	}

	regfree(&processIdRgx);
	closedir(procDirectory);
	return processIds;
}

void destroyProcessIds(uint32_t *pidList) {

	if (!pidList) free(pidList);

}


ProcessCollection* createProcessCollection() {

	size_t size;
	ProcessCollection *rProcesses;
	uint32_t *pids;

	pids = getProcessIds(&size);
	if (!pids) {
		fprintf(stderr, "ERROR : Failed to get process ids.\n");
		return NULL;
	}

	if (size <= 0) {
		fprintf(stderr, "ERROR : Failed to create process collection with size argument %zu.\n", size);
		destroyProcessIds(pids);
		return NULL;
	}

	rProcesses = (ProcessCollection*) malloc(sizeof(ProcessCollection));
	if (!rProcesses) {
		fprintf(stderr, "ERROR : Failed to allocate memory for process collection.\n");
		destroyProcessIds(pids);
		return NULL;
	}

	rProcesses->processes = (Process*) malloc(sizeof(Process) * (size+1));
	if (!rProcesses->processes) {
		fprintf(stderr, "ERROR : Failed to allocate memory for processes in the process collection. size : %zu\n", size);
		free(rProcesses);
		destroyProcessIds(pids);
		return NULL;
	}

	rProcesses->size = size + 1;

	for (int i = 0; i < rProcesses->size; i++)
		rProcesses->processes[i].processId = pids[i]; 

	return rProcesses;
	
}

void destroyProcessCollection(ProcessCollection *rProcesses) {

	if (rProcesses != NULL) {
		if (rProcesses->processes != NULL)
			free(rProcesses->processes);
		free(rProcesses);
	}

}

int parseProcessStats(Process *p) {

	char processStatPath[50], *processStatContent;
	uint32_t waitOne, waitTwo;

	if (!p) {
		fprintf(stderr, "ERROR : Failed to parse process CPU statistics. Process is (null).\n");
		return -1;
	}

	sprintf(processStatPath, "/%s/%u/%s", "proc", p->processId, "stat");

	FILE *processStatFile = fopen(processStatPath, "r");
	if (!processStatFile) {
		fprintf(stderr, "WARNING : Failed to open %s. File not found or permissions denied.\n", processStatPath);
		p->userTime = 0;
		p->sysTime = 0;
		p->waitTime = 0;
		return 0;
	}

	processStatContent = (char*) malloc(PROCSTAT_SIZE_MAX);
	if (!processStatContent) {
		fprintf(stderr, "ERROR : Failed to allocate memory for content of %s.\n", processStatPath);
		return -1;
	}

	fread(processStatContent, PROCSTAT_SIZE_MAX, 1, processStatFile);

	sscanf(processStatContent, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %u %u %u %u", 
			&p->userTime, &p->sysTime, 
			&waitOne, &waitTwo);
	p->waitTime = waitOne + waitTwo;

	free(processStatContent);
	fclose(processStatFile);
	return 0;
}

static int isLineRss(const char * line) {

	regex_t rssRgx;
	char *rssPattern = "^Rss", *lineCopy;
	int err;

	if (regcomp(&rssRgx, rssPattern, REG_EXTENDED)) {
		fprintf(stderr, "ERROR : Failed to compile regex pattern %s.\n", rssPattern);
		return -1;
	}

	lineCopy = (char*) malloc(strlen(line) + 1);
	if (!lineCopy) {
		fprintf(stderr, "ERROR : Couldn't allocate memory for a copy of line :\n%s.\n", line);
		regfree(&rssRgx);
		return -1;
	}
	strncpy(lineCopy, line, strlen(line) + 1);
	lineCopy[strlen(line)] = '\0';

	if (regexec(&rssRgx, lineCopy, 0, NULL, 0) != REG_NOMATCH) {
		free(lineCopy);
		regfree(&rssRgx);
		return 1;
	}

		free(lineCopy);
		regfree(&rssRgx);
		return 0;
}

int parseProcessMem(Process *p) {

	char *processSMapsContent, *processSMapsContentCopy,
	     processSMapsPath[50], *line, *saveptr;
	FILE *processSMapsFile;
	uint32_t tempResMem;

	if (!p) {
		fprintf(stderr, "ERROR : Failed to parse process CPU statistics. Process is (null).\n");
		return -1;
	}
	p->resMem = 0;

	sprintf(processSMapsPath, "/proc/%u/smaps", p->processId);

	processSMapsFile = fopen(processSMapsPath, "r");
	if (!processSMapsFile) {
		fprintf(stderr, "WARNING : Failed to open file %s. File not found or permissions denied.\n", processSMapsPath);
		p->resMem = 0;
		return 0;
	}

	processSMapsContent = (char*) malloc(PROCSMAP_SIZE_MAX);
	if (!processSMapsContent) {
		fprintf(stderr, "ERROR : Failed to allocate memory for the content of %s.\n", processSMapsPath);
		fclose(processSMapsFile);
		return -1;
	}

	processSMapsContentCopy = (char*) malloc(PROCSMAP_SIZE_MAX);
	if (!processSMapsContentCopy) {
		fprintf(stderr, "ERROR : Failed to allocate memory for the copy of the content of %s.\n", processSMapsPath);
		fclose(processSMapsFile);
		free(processSMapsContent);
		return -1;
	}

	fread(processSMapsContent, PROCSMAP_SIZE_MAX, 1, processSMapsFile);
 
	strncpy(processSMapsContentCopy, processSMapsContent, PROCSMAP_SIZE_MAX);

	line = strtok_r(processSMapsContent, "\n", &saveptr);
	if (!line) {
		fprintf(stderr, "WARNING : %s is empty.\n", processSMapsPath);
		fclose(processSMapsFile);
		free(processSMapsContent);
		free(processSMapsContentCopy);
		return 0;
	}

	while (line != NULL) {
		if (isLineRss(line)) {
			sscanf(line, "%*s %u", &tempResMem);
			p->resMem += tempResMem;
		}

		line = strtok_r(NULL, "\n", &saveptr);
	}
	

	fclose(processSMapsFile);
	free(processSMapsContent);
	free(processSMapsContentCopy);
	return 0;
}

int parseProcessCmd(Process *p) {

	char processCommand[COMMAND_PATH_LENGTH], processCommandPath[50];
	FILE *processCommandFile;

	if (!p) {
		fprintf(stderr, "ERROR : Failed to parse process CPU statistics. Process is (null).\n");
		return -1;
	}

	sprintf(processCommandPath, "/proc/%u/comm", p->processId);
	processCommandFile = fopen(processCommandPath, "r");
	if (!processCommandFile) {
		fprintf(stderr, "WARNING : Failed to open file %s. File not found or permissions denied.\n", processCommandPath);
		p->command[0] = '\0';
		return 0;
	}

	fread(processCommand, COMMAND_PATH_LENGTH, 1, processCommandFile);

	strncpy(p->command, processCommand, strlen(processCommand));

	size_t commandLength = strlen(p->command);
	for (int i = 0; i < commandLength; i++) {
		if (p->command[i] == '\n') {
			p->command[i] = '\0';
			break;
		}
	}

	fclose(processCommandFile);
	return 0;
}

int parseProcessCollection(ProcessCollection *rProcesses) {

	if (!rProcesses) {
		fprintf(stderr, "ERROR : Failed to parse process collection. Invalid arguments (null).\n");
		return -1;
	} else if (!rProcesses->processes && rProcesses->size <= 0) {
		fprintf(stderr, "ERROR : Failed to parse processes. Processes array is (null).\n");
		return -1;
	}

	int err;

	for (int i = 0; i < rProcesses->size; i++) {
		err = parseProcessStats(&rProcesses->processes[i]);
		if (err == -1) {
			fprintf(stderr, "ERROR : Failed to parse processes collection. CPU stats parsing error.\n");
			return -1;
		}
		err = parseProcessMem(&rProcesses->processes[i]); 
		if (err == -1) {
			fprintf(stderr, "ERROR : Failed to parse processes collection. Memory info parsing error.\n");
			return -1;
		}
		err = parseProcessCmd(&rProcesses->processes[i]); 
		if (err == -1) {
			fprintf(stderr, "ERROR : Failed to parse processes collection. Command name parsing error.\n");
			return -1;
		}
	}

	return 0;
}
