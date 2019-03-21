#pragma once
#ifndef AM335X_CONSOLE_H
#define AM335X_CONSOLE_H
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
 * Abstract: 	AM335x Serial Interface for newlib system calls
 *
 * Purpose:	This module provides services for the serial user interface
 * 		used by the standard input/output functions on UART0
 *
 * Author: 	Daniel Gachet
 * Date: 	03.07.2015
 */

#include <am335x_uart.h>

/** 
 * initialization method, this method shall be called prior any other ones 
 */
static inline void am335x_console_init(void) 
{
	am335x_uart_init (AM335X_UART0); 
}

/**
 * method to change the baudrate
 *
 * @param baudrate terminal baudrate value in bit/s
 */
static inline void am335x_console_set_baudrate (uint32_t baudrate)
{
	am335x_uart_set_baudrate (AM335X_UART0, baudrate);
}


/** 
 * method to test if a character is available 
 *
 * @return true if character available, false otherwise
 */
static inline bool am335x_console_tstc(void)
{
	return am335x_uart_tstc (AM335X_UART0);
}


/**
 * method to read a character from the serial interface.
 * the caller will be suspended until a character is available
 *
 * @return character received on the serial interface
 */
static inline int am335x_console_getc(void)
{
	return am335x_uart_read (AM335X_UART0);
}


/**
 * methods to send characters on the serial interface.
 * notice: \n will be followed by \r
 *
 * @param c character to send on the serial interface
 */
static inline void am335x_console_putc (char c)
{
	am335x_uart_write (AM335X_UART0, c);
	if (c == '\n')
		am335x_uart_write (AM335X_UART0, '\r');
	if (c == '\r')
		am335x_uart_write (AM335X_UART0, '\n');
}


/**
 * methods to send a character string on the serial interface.
  *
 * @param s character string to send on the serial interface
 */
static inline void am335x_console_puts (const char *s)
{
	while (*s)
		am335x_console_putc (*s++);
}

#endif

