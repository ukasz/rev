#!/bin/bash

#Build the test
make

# Check that the exec was built...
if [ -f ex4.exe ]; then
  sst ./rev-test-ex4.py
else
  echo "Test EX4: ex4.exe not Found - likely build failed"
  exit 1
fi 
