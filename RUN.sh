#!/bin/bash

EXE="./SystemMonitor"

# Check if the executable exists
if [ -f "$EXE" ]; then
    sudo "$EXE" 2> /dev/null
    exit $?
else
    echo "Please build SystemMonitor using 'make'"
    exit 1
fi

