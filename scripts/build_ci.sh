#!/usr/bin/env bash

set -e

BUILD=build
VERBOSE=1

mkdir -p ${BUILD}
cd ${BUILD}
cmake -DCMAKE_BUILD_TYPE=Debug ..
make VERBOSE=1 max_comsubseq -j4
make check
