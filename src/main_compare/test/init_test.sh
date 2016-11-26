#!/usr/bin/env bash
mkdir -p testoutput

git submodule init
git submodule update

cd gtest/googletest
cmake .
make -j4

