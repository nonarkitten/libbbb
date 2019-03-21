#!/bin/bash

rm -Rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi.cmake ..
make
cpack --config CPackConfig.cmake
cpack --config CPackSourceConfig.cmake
