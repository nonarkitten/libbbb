# System library for the Beaglebone Black

This project is the system library (`libbbb`) for the Beaglebone Black used in the lecture "Embedded Systems 1 & 2" at the "Haute école d'ingénierie et d'architecture" of Fribourg.

## Building

```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi.cmake ..
make
```

## Downloading

You can download the latest automatic build [here](https://gitlab.forge.hefr.ch/embedded-systems-1-2/libbbb/-/jobs/artifacts/master/browse/build?job=build).
