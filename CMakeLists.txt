cmake_minimum_required(VERSION 3.8)
project(libbbb)

set(CMAKE_C_STANDARD 11)
enable_language(ASM)

set(HEADER_FILES
        src/am335x_clock.h
        src/am335x_console.h
        src/am335x_gpio.h
        src/am335x_i2c.h
        src/am335x_mux.h
        src/am335x_spi.h
        src/am335x_uart.h
        src/string2.h)

set(SOURCE_FILES
        ${HEADER_FILES}
        src/am335x_clock.c
        src/am335x_gpio.c
        src/am335x_i2c.c
        src/am335x_mux.c
        src/am335x_spi.c
        src/am335x_uart.c
        src/bbb_syscall.c
        src/crt0.S
        src/string2.S)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

add_library(bbb ${SOURCE_FILES})
target_include_directories(bbb PRIVATE src)

install(TARGETS bbb DESTINATION lib)
install(FILES ${HEADER_FILES} DESTINATION include)

set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "System library for beaglebone black")
set(CPACK_PACKAGE_VENDOR "Haute école d'ingénierie et d'architecture de Fribourg")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set (CPACK_SOURCE_IGNORE_FILES "/upstream/;/build/;/.git*;/.idea/;/*.sh")
set(CPACK_SYSTEM_NAME "arm-none-eabi")
include(CPack)
