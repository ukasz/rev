#!/bin/bash

#Build the test
make

# Check that the exec was built...
if [ -f dot_double.exe ]; then
  sst ./dot_double.py
else
  echo "Test DOT_DOUBLE: dot_double.exe not Found - likely build failed"
  exit 1
fi 
