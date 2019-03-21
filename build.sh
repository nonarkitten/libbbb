#!/bin/bash

rm -Rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi.cmake ..
make VERBOSE=1
cpack --config CPackConfig.cmake
cpack --config CPackSourceConfig.cmake
