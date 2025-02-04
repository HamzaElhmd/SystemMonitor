#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Mem.h"

int parseMemStat(MemStat *memInfo) {

	FILE *memInfoFile;
	char *line, *memInfoPath = "/proc/meminfo";
	size_t len = 0;

	if (!memInfo) {
		fprintf(stderr, "ERROR : Can't parse NULL MemStat.\n");
		return -1;
	}

	memInfoFile = fopen(memInfoPath, "r");
	if (!memInfoFile) {
		fprintf(stderr, "ERROR : Failed to open %s. File not found or permissions denied.\n", memInfoPath);
		return -1;
	}

	while (getline(&line, &len, memInfoFile) != -1) {

		if (strncmp(line, "MemTotal", 8) == 0) {
			sscanf(line, "%*s %"PRIu32, &memInfo->MemTotal);
		} else if (strncmp(line, "MemFree", 7) == 0) {	
			sscanf(line, "%*s %"PRIu32, &memInfo->MemFree);
		} else if (strncmp(line, "Cached", 6) == 0) {
			sscanf(line, "%*s %"PRIu32, &memInfo->Cached);
		} else if (strncmp(line, "SwapTotal", 9) == 0) {
			sscanf(line, "%*s %"PRIu32, &memInfo->SwapTotal);
		} else if (strncmp(line, "SwapFree", 8) == 0) {
			sscanf(line, "%*s %"PRIu32, &memInfo->SwapFree);
		}

	}

	free(line);
	fclose(memInfoFile);
	return 0;
}
