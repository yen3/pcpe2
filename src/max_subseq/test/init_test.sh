#!/usr/bin/env bash
mkdir -p testoutput

#git submodule init
#git submodule update

git clone https://github.com/google/googletest gtest

cd gtest/googletest
cmake .
make -j4

