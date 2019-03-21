#pragma once
#ifndef AM335X_I2C_H
#define AM335X_I2C_H
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
 * Abstract: 	AM335x I2C Driver 
 *
 * Purpose:	This module implements basic services to drive the AM335x I2C
 *		controller.
 *
 * Author: 	Daniel Gachet
 * Date: 	03.07.2015
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * defines the available i2c controllers of the am335x
 */
enum am335x_i2c_controllers {
	AM335X_I2C0, 
//	AM335X_I2C1, 	--> no more supported on HEIA-FR board
	AM335X_I2C2, 
};

/** 
 * method to initialize a specific am335x i2c controller,
 * this method should be called prior any other method.
 *
 *@param ctrl am335x i2c controller name
 *@bus_speed i2c bus speed in Hz
 */
extern void am335x_i2c_init(
	enum am335x_i2c_controllers ctrl,
	uint32_t bus_speed);


/**
 * method to read data bytes from the internal register files of
 * the specified chip.
 * 
 *@param chip_id chip identification, its address
 *@param reg internal chip register address from which the data byte should be read
 *@param data data buffer containing the read data bytes
 *@param data_len number of data byte to read
 * 
 *@return int status, 0=success, -1=error
 */
extern int am335x_i2c_read(
	enum am335x_i2c_controllers ctrl,
	uint8_t chip_id, 
	uint8_t reg,
	uint8_t* data, 
	uint16_t data_len);


/**
 * method to write data byte into the internal register files of 
 * the specified chip. 
 *  
 *@param chip_id chip identification, its address
 *@param reg internal chip register into which the data byte should be written
 *@param cmd_sz size of of the command word
 *@param buffer data buffer containing the data bytes to write
 *@param buffer_len number of data byte to write
 * 
 *@return int status, 0=success, -1=error
 */
extern int am335x_i2c_write(
	enum am335x_i2c_controllers ctrl,
	uint8_t chip_id, 
	uint8_t reg,
	const uint8_t* data, 
	uint16_t data_len);


/**
 * method to see if a chip is present on the I2C bus.
 * 
 *@param chip_id chip identification, its address
 *
 *@return bool true if the chip is present, false otherwise
 */
extern bool am335x_i2c_probe(
	enum am335x_i2c_controllers ctrl,
	uint8_t chip_id);

#endif

