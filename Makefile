CC = gcc

# src path directory for source files
SRC_DIR = src

# Include path directory for header files
INCLUDE_DIR = include

# Build path directory for object files
BUILD_DIR = build

# List of source files in the src path directory 
SRCS = $(SRC_DIR)/Cpu.c $(SRC_DIR)/Mem.c $(SRC_DIR)/Process.c $(SRC_DIR)/SystemMonitor.c

# List of object files resulting from compilation of source files in src path directory
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Executable file name
EXE = SystemMonitor

# GCC compiler flags
CFLAGS = -I$(INCLUDE_DIR) -Wall -g -Wextra

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(EXE)
