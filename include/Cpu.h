#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <sys/types.h>

/* /proc/stat file lists the time the cpu (and cores) has spent in: */
/*
 * normal processes (user mode)
 * nice processes
 * system processes (kernel mode)
 * idle  
 * I/O wait time (waiting for IO) 
 * */

typedef struct {

	uint32_t userTime;
	uint32_t sysTime;
	uint32_t idleTime;
	uint32_t ioWaitTime;
	uint16_t processorId; /* Processor ID: refers to the logical core number */
} Cpu;

typedef struct {
	Cpu *processors;
	size_t size;
} CpuCollection;

/* Get the number of logical cores of the system (Processor IDs) 
 * Returns the an amount in size_t */
int getNProcessors();

/** Create CPU statistics collection of n cores. 
 * Returns CpuStat array on success, and NULL on error */
CpuCollection* createCpuCollection(int nProcessors);

/** Parse each line of /resources/cpu.txt into a CpuStat structure 
 * Returns 0 on success, and 1 on error */
int parseCpuStatCollection(CpuCollection *cpus);


void printCpuCollection(const CpuCollection *cpus);

/** Free the memory of CPU statistics collection **/
void destroyCpuCollection(CpuCollection *cpus);

/* Calculates CPU usage percentage 
 * return a percentage value between 0.00 and 100.00 on success and -1.00 on error */
double calculateUsagePercentage(Cpu cpuStat);

#endif
