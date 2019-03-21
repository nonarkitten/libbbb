# System library for the Beaglebone Black

This project is the system library (`libbbb`) for the Beaglebone Black used in the lecture "Embedded Systems 1 & 2" at the "Haute école d'ingénierie et d'architecture" of Fribourg.

## Building

```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi.cmake ..
make
```