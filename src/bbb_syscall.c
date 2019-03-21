/**
 * Copyright 2015 University of Applied Sciences Western Switzerland / Fribourg
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
 * Project:	HEIA-FR / Embedded Systems 1+2 Laboratory
 *
 * Abstract: 	newlib system calls
 *
 * Purpose:	This module implements all the required newlib system calls
 *
 * Author: 	Daniel Gachet
 * Date: 	08.07.2015
 */


/* GNU ARM Toolchain & newlib: system calls */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>

#include "am335x_clock.h"
#include "am335x_console.h"
#include "am335x_dmtimer1.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

enum f_modes {UNUSED, NONBLOCK, BLOCK};
static enum f_modes f_mode[10] = {
	[0] = BLOCK,
	[1] = BLOCK,
	[2] = BLOCK,
};

int _open (const char *name, int flags, int mode) 
{
	(void)mode;
	if (strcmp(name, "/dev/console")!=0) return -1;
	unsigned i = 0;
	while ((i < ARRAY_SIZE(f_mode)) && (f_mode[i] != UNUSED)) i++;
	if (i < ARRAY_SIZE(f_mode)) {
		f_mode[i] = (flags & O_NONBLOCK) != 0 ? NONBLOCK : BLOCK;
		return i;
	} 
	return -1;
}

int _read(int file, char *ptr, int len __attribute__((unused))) {
	if ((unsigned)file > ARRAY_SIZE(f_mode)) return -1;
	if ((f_mode[file] == NONBLOCK) & !am335x_console_tstc()) return -1;
	*ptr = am335x_console_getc();
	return 1;
}

int _write(int file __attribute__((unused)), char *ptr, int len) {
	int todo;
	for (todo = 0; todo < len; todo++) {
		am335x_console_putc(*ptr++);
	}
	return len;
}

int _lseek(int file __attribute__((unused)), int ptr __attribute__((unused)), int dir __attribute__((unused))) {
	return 0;
}

int _isatty(int file __attribute__((unused))) {
	return 1;
}

#include <sys/stat.h>
int _fstat(int file __attribute__((unused)), struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int _close(int file) {
	if ((unsigned)file > ARRAY_SIZE(f_mode)) return -1;
	f_mode[file] = UNUSED;
	return 0;
}

#if 0
void pint (int v)
{
	if (v < 0) {
		am335x_console_putc ('-');
		v = -v;
	}
	if (v > 0) pint (v/10);
	am335x_console_putc (v%10+'0');
}

void padr (void* a)
{
	unsigned long v = (unsigned long)a;
	if (v > 0) padr ((void*)(v/16));
	v %= 16;
	am335x_console_putc (v>9 ? v-10+'a' : v+'0');
}
#endif

void* _sbrk(int incr) {
	extern char _end; /* Defined by the linker */
	extern char _stack_bottom;
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
	  heap_end = &_end;
	}
	prev_heap_end = heap_end;
	if (heap_end + incr > &_stack_bottom) {
		//write (1, "Heap and stack collision\n", 25);
		//abort ();
		return 0;
	} else {
		heap_end += incr;
	}
#if 0
	am335x_console_puts("_sbrk= incr="); pint(incr);
	am335x_console_puts(", _end=");      padr (&_end);
	am335x_console_puts(", heap_end=");  padr (heap_end);
	am335x_console_puts(", ret=");       padr (prev_heap_end);
	am335x_console_puts("\n");
#endif
	return (void*) prev_heap_end;
}

void _exit(int i __attribute__((unused)))
{
    while(1);
}

extern void (*__preinit_array_start []) (void) __attribute__((weak));
extern void (*__preinit_array_end []) (void) __attribute__((weak));
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));

void _init()
{
	am335x_clock_enable_l3_l4wkup();
	am335x_console_init();
	am335x_dmtimer1_init();

	size_t count = __preinit_array_end - __preinit_array_start;
	for (size_t i = 0; i < count; i++)
		 __preinit_array_start[i]();

	count = __init_array_end - __init_array_start;
	for (size_t i = 0; i < count; i++)
		__init_array_start[i]();
}

void _fini()
{
		size_t count = __preinit_array_end - __preinit_array_start;
		for (int i = count - 1; i >= 0; i--)
			__fini_array_start[i]();
}

int _getpid ()
{
	return 1;
}

int _kill (int pid __attribute__((unused)), int sig __attribute__((unused)))
{
	while(1);
}

/*
void abort(void)
 {
	 while (1);
 }
*/

