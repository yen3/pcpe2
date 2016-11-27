#!/usr/bin/env bash

err_report() {
    echo "Error on line $1"
}

trap 'err_report $LINENO' ERR

# Build the project
make build

# Build the unit test program
make unit_test
