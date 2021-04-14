/**
 * Copyright 2019 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Project: HEIA-FR / Embedded Systems 1+2 Laboratory
 *
 * Abstract: newlib system calls
 *
 * Purpose: This module implements all the required newlib system calls
 *
 * Author:  Daniel Gachet
 * Date:        08.05.2019
 */

/* GNU ARM Toolchain & newlib: system calls */
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bbb/am335x_clock.h"
#include "bbb/am335x_console.h"
#include "bbb/am335x_dmtimer1.h"
#include "bbb/bbb_fs.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
//#define pr_debug(x...) printf(x)
#define pr_debug(x...)

// --- file system services ---------------------------------------------------

// all the file and directory access method have been
// moved to the fs.c file

// --- miscellaneous services -------------------------------------------------

void* _sbrk(int incr)
{
    extern char _end; /* Defined by the linker */
    extern char _stack_bottom;
    static char* heap_end;
    char* prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;
    if (heap_end + incr > &_stack_bottom) {
        return 0;
    } else {
        heap_end += incr;
    }
    return (void*)prev_heap_end;
}

void _exit(int i __attribute__((unused)))
{
    while (1)
        ;
}

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));
extern void (*__fini_array_start[])(void) __attribute__((weak));
extern void (*__fini_array_end[])(void) __attribute__((weak));

void _init()
{
    am335x_clock_enable_l3_l4wkup();
    am335x_dmtimer1_init();
    bbb_fs_init();

    size_t count = __preinit_array_end - __preinit_array_start;
    for (size_t i = 0; i < count; i++) __preinit_array_start[i]();

    count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; i++) __init_array_start[i]();
}

void _fini()
{
    size_t count = __preinit_array_end - __preinit_array_start;
    for (int i = count - 1; i >= 0; i--) __fini_array_start[i]();
}

int _getpid() { return 1; }

int _kill(int pid __attribute__((unused)), int sig __attribute__((unused)))
{
    while (1)
        ;
}
