#!/bin/bash

rm -Rf build
mkdir build
cd build
cmake ..
make VERBOSE=1
cpack --config CPackConfig.cmake
cpack --config CPackSourceConfig.cmake
