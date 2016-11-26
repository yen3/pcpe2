#!/usr/bin/env bash

err_report() {
    echo "Error on line $1"
}

trap 'err_report $LINENO' ERR

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
make test
