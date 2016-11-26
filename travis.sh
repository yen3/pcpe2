#!/usr/bin/env bash

WORK_DIR=`pwd`

# Build the project
cd ${WORK_DIR}
cd src/main_compare
make -j4

# Build the unit test program
cd ${WORK_DIR}
cd src/main_compare/test
./init_test.sh
make -j4




