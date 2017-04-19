#!/usr/bin/env bash
mkdir -p testoutput

if [ ! -d "gtest" ]; then
    git clone https://github.com/google/googletest gtest
    cd gtest/googletest
    cmake .
    make -j4
fi

