#pragma once
#ifndef AM335X_SPI_H
#define AM335X_SPI_H
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
 * Project: HEIA-FR / Embedded Systems 1+2 Laboratory
 *
 * Abstract: AM335x McSPI Driver
 *
 * Purpose: This module implements basic services to drive the AM335x McSPI
 *          controller.
 *
 * Author:  Daniel Gachet
 * Date:    24.08.2015
 */

#include <stdint.h>
#include <stdlib.h>

/**
 * defines the available spi controllers of the am335x
 */
enum am335x_spi_controllers {
    AM335X_SPI0,
    AM335X_SPI1,
};

/**
 * defines the available channels to be activated during data transfer
 */
enum am335x_spi_channels {
	AM335X_CHAN0,
    AM335X_CHAN1,
};

/**
 * method to initialize a specific am335x spi controller,
 * this method should be called prior any other method.
 *
 *@param ctrl am335x spi controller name
 *@param channel channel to be activated during transfer
 *@bus_speed i2c bus speed in Hz
 *@param word_len size of data word in bits
 */
extern void am335x_spi_init(
		enum am335x_spi_controllers ctrl,
		enum am335x_spi_channels channel,
		uint32_t bus_speed,
		uint32_t word_len);

/**
 * method to transfer data bytes to and from the specified chip.
 *
 *@param ctrl am335x spi controller name
 *@param channel channel to be activated during transfer
 *@param buffer data buffer containing the data to send
 *       and to receive into
 *@param buffer_len number of data bytes to read & write
 *
 *@return int status, 0=success, -1=error
 */
extern int am335x_spi_xfer(
		enum am335x_spi_controllers ctrl,
        enum am335x_spi_channels channel,
        uint8_t* buffer,
        size_t buffer_len);

#endif
