#include <stdio.h>
#include <inttypes.h>
#include "../include/Mem.h"

int main(int argc, char *argv[]) {

	MemStat memInfo;
	int err;

	err = parseMemStat(&memInfo);
	if (err == -1) {
		fprintf(stderr, "An error occured. Aborting...\n");
		return 1;
	}

	printf("Total Memory: %"PRIu32"\n", memInfo.MemTotal);
	printf("Free Memory: %"PRIu32"\n", memInfo.MemFree);
	printf("Cached Memory: %"PRIu32"\n", memInfo.Cached);
	printf("Total Swap Memory: %"PRIu32"\n", memInfo.SwapTotal);
	printf("Free Swap Memory: %"PRIu32"\n", memInfo.SwapFree);

	return 0;
}
