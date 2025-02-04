#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <sys/types.h>

#define COMMAND_PATH_LENGTH 256

/* A process is defined as having the following fields to show
 * statistics : 
 * 1. Process ID (also known as <pid>) 
 * 2. User Time (Time process ran in user mode)
 * 3. Kernel/System Time (Time process ran in kernel mode)
 * 4. Wait Time (Time a process waited for a chiled process to finish)
 * 5. Resident RAM Memory (Physical memory proportion allocated by the process)
 * 6. Command (Name of the command used to run the process) */
typedef struct {
	uint32_t processId;
	uint32_t userTime;
	uint32_t sysTime;
	uint32_t waitTime;
	uint32_t resMem;
	char command[COMMAND_PATH_LENGTH];
}Process;

typedef struct {
	Process *processes;
	size_t size;
} ProcessCollection;

/* Get the list of process IDs from the /proc directory
 * Returns a heap allocated array of process IDs in success 
 * Returns NULL in error (I/O || Parsing) */
uint32_t* getProcessIds(size_t *size);
void destroyProcessIds(uint32_t *pidList); /* Free the heap allocated memory */

ProcessCollection* createProcessCollection();
void destroyProcessCollection(ProcessCollection *rProcesses);

/* Get the cpu statistics (user and kernel mode usage and wait times) of 
 * the process /proc/<pid>/stat 
 * Returns 0 in success, and -1 in error (I/O || Parsing) */
int parseProcessStats(Process *p);

/* Get the resident memory of the process in the memory by summing
 * all RSS fields in /proc/<pid>/smaps 
 * Returns 0 in success, and -1 in error (I/O || Parsing )*/
int parseProcessMem(Process *p);

/* Get the command used to run the process p from /proc/comm 
 * Returns 0 in success, and -1 in error */
int parseProcessCmd(Process *p);

int parseProcessCollection(ProcessCollection *rProcesses);

#endif
