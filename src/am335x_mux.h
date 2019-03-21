#pragma once
#ifndef AM335X_MUX_H
#define AM335X_MUX_H
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
 * Abstract: 	AM335x Pin Multiplexer Driver 
 *
 * Purpose:	This module implements basic services to drive the AM335x 
 * 		pin multiplexer module.
 *
 * Author: 	Daniel Gachet
 * Date: 	03.08.2016
 */

#include <stdint.h>
#include <stdbool.h>


/**
 * am335x uart mux modules
 */
enum am335x_mux_uart_modules {
	AM335X_MUX_UART0,
	AM335X_MUX_UART1,
	AM335X_MUX_UART2,
	AM335X_MUX_UART3,
	AM335X_MUX_UART4,
	AM335X_MUX_UART5,
};

/**
 * am335x i2c mux modules
 */
enum am335x_mux_i2c_modules {
	AM335X_MUX_I2C0,
//	AM335X_MUX_I2C1,  --> no more supported on HEIA-FR board
	AM335X_MUX_I2C2,
};

/**
 * am335x spi mux modules
 */
enum am335x_mux_spi_modules {
	AM335X_MUX_SPI0,
	AM335X_MUX_SPI1,
};

/**
 * am335x mmc mux modules
 */
enum am335x_mux_mmc_modules {
	AM335X_MUX_MMC0,
	AM335X_MUX_MMC1,
//	AM335X_MUX_MMC2,  --> not supported on HEIA-FR baord
};


/**
 * am335x epwm mux modules
 */
enum am335x_mux_epwm_modules {
//	AM335X_MUX_EPWM0, --> not supported on HEIA-FR Board
	AM335X_MUX_EPWM1,
	AM335X_MUX_EPWM2,  
};

/**
 * am335x gpio clock modules, pin directions and pull-up/-down configuration
 */
enum am335x_mux_gpio_modules {
	AM335X_MUX_GPIO0,
	AM335X_MUX_GPIO1,
	AM335X_MUX_GPIO2,
	AM335X_MUX_GPIO3,
};

enum am335x_mux_gpio_pin_direction {
	AM335X_MUX_PIN_OUT,
	AM335X_MUX_PIN_IN,
};

enum am335x_mux_gpio_pin_pull {
	AM335X_MUX_PULL_NONE,
	AM335x_MUX_PULL_UP,
	AM335X_MUX_PULL_DOWN,
};


/**
 * method to setup uart pins for use. 
 *          
 * @param   module uart controller name (instance number)
 */
extern void am335x_mux_setup_uart_pins (enum am335x_mux_uart_modules module);


/**
 * method to setup i2c pins for use. 
 *          
 * @param   module i2c controller number (instance number)
 */
extern void am335x_mux_setup_i2c_pins (enum am335x_mux_i2c_modules module);


/**
 * method to setup spi pins for use. 
 *          
 * @param   module spi controller number (instance number)
 */
extern void am335x_mux_setup_spi_pins (enum am335x_mux_spi_modules module);


/**
 * method to setup mmc pins for use.
 *
 * @param   module spi controller number (instance number)
 */
extern void am335x_mux_setup_mmc_pins (enum am335x_mux_mmc_modules module);


/**
 * method to setup mmc pins for use.
 *
 * @param   module spi controller number (instance number)
 */
extern void am335x_mux_setup_epwm_pins (enum am335x_mux_epwm_modules module);


/**
 * method to setup cpsw (ethernet) pins for use.
 */
extern void am335x_mux_setup_cpsw_pins();


/**
 * method to setup pin multiplexing as gpio 
 *
 *@param module gpio module name
 *@param pin_nr number of the I/O pin
 */
extern void am335x_mux_setup_gpio_pin(
	enum am335x_mux_gpio_modules module, 
	uint32_t pin_nr,
	enum am335x_mux_gpio_pin_direction pin_dir,
	enum am335x_mux_gpio_pin_pull pin_pull);


#endif

