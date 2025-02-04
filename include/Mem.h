#ifndef MEM_H
#define MEM_H

#include <stdint.h>

/* Memory statistics is taken from the /proc/meminfo dynamic file 
 * It consists of many memory related fields. 
 * To compute the memory usage, we take into account the following : 
 * 1. Total memory of the system 
 * 2. Free memory unused by the system 
 * 3. Total swap memory (Operating system stores inactive data in the hard disk)
 * 4. Free swap memory 
 * 5. Cached memory */
typedef struct {

	uint32_t MemTotal;
	uint32_t MemFree;
	uint32_t SwapTotal;
	uint32_t SwapFree;
	uint32_t Cached;

} MemStat;

/* Parse a MemStat structure from the /proc/meminfo file 
 * Returns 0 in success, 1 in error */
int parseMemStat(MemStat *memInfo);

#endif

