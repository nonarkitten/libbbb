#pragma once
#ifndef AM335X_UART_H
#define AM335X_UART_H
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
 * Abstract: 	AM335x Universal Asynchronous Receiver/Transmitter (UART) Driver 
 *
 * Purpose:	This module implements basic services to drive the AM335x UART
 *		controller as serial user interface mainly used by the standard 
 *		input/output functions of the standard C library.
 *
 * Author: 	Daniel Gachet
 * Date: 	03.07.2015
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * defines the available uart controllers of the am335x
 */
enum am335x_uart_controllers {
	AM335X_UART0, 
	AM335X_UART1, 
	AM335X_UART2, 
	AM335X_UART3, 
	AM335X_UART4, 
	AM335X_UART5, 
};

/** 
 * method to initialize a specific am335x controller to work as a serial line.
 * by default the line will be configured as follow:
 *   - databits    : 8
 *   - stop bit    : 1
 *   - parity      : none
 *   - baudrate    : 115200
 *   - flowcontrol : none 
 *
 * @param ctrl am335x uart controller number
 */
extern void am335x_uart_init (enum am335x_uart_controllers ctrl);


/**
 * method to change the baudrate
 *
 * @param ctrl am335x uart controller number
 * @param baudrate terminal baudrate value in bit/s
 */
extern void am335x_uart_set_baudrate (
	enum am335x_uart_controllers ctrl, 
	uint32_t baudrate);


/** 
 * method to test if a character is available 
 *
 * @param ctrl am335x uart controller number
 * @return true if character available, false otherwise
 */
extern bool am335x_uart_tstc(enum am335x_uart_controllers ctrl);


/**
 * method to read a character from the serial interface.
 * the caller will be blocked until a character is available.
 *
 * @param ctrl am335x uart controller number
 * @return character received on the serial interface
 */
extern int am335x_uart_read(enum am335x_uart_controllers ctrl);


/**
 * method to send a character on the serial interface.
 * the caller will be blocked until the character can be put into the uart fifo.
 *
 * @param ctrl am335x uart controller number
 * @param c character to send on the serial interface
 */
extern void am335x_uart_write(
	enum am335x_uart_controllers ctrl,
	int c);

#endif

