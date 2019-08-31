# Copyright 2019 Jacques Supcik, HEIA-FR
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# CMake file for libbbb
#

cmake_minimum_required(VERSION 3.8)
project(libbbb)

set(CMAKE_C_STANDARD 11)
enable_language(ASM)

set(HEADER_FILES
        src/bbb/am335x_clock.h
        src/bbb/am335x_console.h
        src/bbb/am335x_dmtimer1.h
        src/bbb/am335x_gpio.h
        src/bbb/am335x_i2c.h
        src/bbb/am335x_mux.h
        src/bbb/am335x_spi.h
        src/bbb/am335x_uart.h
        src/bbb/font_8x8.h
        src/bbb/oled.h
        src/bbb/string2.h
	src/bbb/bbb_fs.h
	src/bbb/console.h
	src/bbb/dirent.h
	src/bbb/path.h
	src/bbb/vfs.h)

set(SOURCE_FILES
        ${HEADER_FILES}
        src/bbb/am335x_clock.c
        src/bbb/am335x_dmtimer1.c
        src/bbb/am335x_gpio.c
        src/bbb/am335x_i2c.c
        src/bbb/am335x_mux.c
        src/bbb/am335x_spi.c
        src/bbb/am335x_uart.c
        src/bbb/bbb_syscall.c
        src/bbb/crt0.S
        src/bbb/oled.c
        src/bbb/string2.S
	src/bbb/bbb_fs.c
	src/bbb/console.c
	src/bbb/dirent.c
	src/bbb/path.c
	src/bbb/vfs.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

add_library(bbb ${SOURCE_FILES})
target_include_directories(bbb PRIVATE src)

install(TARGETS bbb DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include/bbb)

set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "System library for beaglebone black")
set(CPACK_PACKAGE_VENDOR "Haute école d'ingénierie et d'architecture de Fribourg")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "3")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_SOURCE_IGNORE_FILES "/upstream/;/build/;/.git*;/.idea/;/.vscode/;/*.sh")
set(CPACK_SYSTEM_NAME "arm-none-eabi")
include(CPack)
