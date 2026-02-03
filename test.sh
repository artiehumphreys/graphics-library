#!/bin/zsh

cd build

cmake ..
make

ctest --output-on-failure
