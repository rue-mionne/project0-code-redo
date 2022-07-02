#!/bin/sh
gcc -Wall -o test unittests.c aggregationSuite.c ../core/aggregationModule.c -lcunit
