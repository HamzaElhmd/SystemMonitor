# Performance Monitor for Linux Systems

A **command-line tool** for monitoring **CPU, memory, and process activity** on **Linux systems**, specifically designed for **Linux kernel 6.12.11**.

## Components

This tool reads from the _/proc_ dynamic filesystem and extracts system information using **basic text processing techniques from the C standard librar**y.

### 1. CPU Monitoring

Reads data from _/proc/stat_ to extract CPU-related information:

* Detects the number of **CPU cores** in the system.
* Parses CPU time spent in different modes (measured in **Jiffies**):
    * **User mode**
    * **Kernel mode**
    * **Idle**
    * **I/O wait**
* Computes **CPU usage (%)**, ignoring **nice process times** and **interrupt times** for simplicity.

### 2. Memory Monitoring

Reads data from _/proc/meminfo_ to extract memory-related statistics:

* Parses memory fields (in **Kilobytes**):
    * **Total Memory**
    * **Free Memory**
    * **Total Swap Memory**
    * **Free Swap Memory**
    * **Buffered Memory**
    * **Cached Memory**
* Computes the following values (converted to **Gigabytes**):
    * **Used Memory**
    * **Used Swap Memory**

### 3. Process Monitoring

Reads from the _/proc_ directory to list running processes:

* Extracts all **process IDs (PIDs)** from _/proc/_.
* Parses **process execution times** from _/proc/[pid]/stat_:
    * **User mode time**
    * **Kernel mode time**
    * **Waiting time (for child processes)**
* Computes **Resident Set Size (RSS) memory usage** by aggregating values from _/proc/[pid]/smaps_.
* Retrieves the **command used to run each process** from _/proc/[pid]/comm_.

## Build and Run

1. Navigate to the project's root directory where the _Makefile_ is located.
2. Compile the project using _make_ (no external dependencies required beyond the **C standard library**).
3. Run the program using the provided script:
```bash
make
./RUN.sh
```
> Note: The _RUN.sh_ script requires **root privileges** since accessing _/proc_ files with detailed process information requires **special permissions**. Running the program without _sudo_ may result in restricted access or missing data.

