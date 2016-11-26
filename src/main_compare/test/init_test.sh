#!/usr/bin/env bash
git submodule init
git submodule update
cd gtest/googletest
cmake .
make -j4
