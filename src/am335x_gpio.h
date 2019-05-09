#pragma once
#ifndef AM335X_GPIO_H
#define AM335X_GPIO_H
/**
 * Copyright 2018 University of Applied Sciences Western Switzerland / Fribourg
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
 * Abstract: AM335x Pin Multiplexer Driver
 *
 * Purpose: This module implements basic services to drive the AM335x
 *          pin multiplexer module.
 *
 * Author:  Daniel Gachet
 * Date:    21.03.2018
 */

#include <stdbool.h>
#include <stdint.h>

/**
 * am335x gpio modules
 */
enum am335x_gpio_modules {
    AM335X_GPIO0,
    AM335X_GPIO1,
    AM335X_GPIO2,
    AM335X_GPIO3,
    AM335X_GPIO_NB_MODULES
};

/**
 * am335x gpio pin direction
 */
enum am335x_gpio_pin_direction {
    AM335X_GPIO_PIN_OUT,
    AM335X_GPIO_PIN_IN,
};

/**
 * am335x gpio pin pull-up, pull-down
 */
enum am335x_gpio_pin_pull {
    AM335X_GPIO_PULL_NONE,
    AM335X_GPIO_PULL_UP,
    AM335X_GPIO_PULL_DOWN,
};

/**
 * am335x gpio pin interrupt modes
 */
enum am335x_gpio_interrupt_modes {
    AM335X_GPIO_IRQ_RISING,
    AM335X_GPIO_IRQ_FALLING,
    AM335X_GPIO_IRQ_BOTH,
    AM335X_GPIO_IRQ_HIGH,
    AM335X_GPIO_IRQ_LOW
};

/**
 * Prototype of the interrupt handler routine
 *
 * @param module gpio module name
 * @param pin_nr number of the I/O pin
 * @param param application specific parameter
 */
typedef void (*am335x_gpio_handler_t)(enum am335x_gpio_modules module,
                                      uint32_t pin_r,
                                      void* param);

/**
 * method to initialize a gpio module
 *
 *@param module gpio module name
 *@return module base address
 */
extern void* am335x_gpio_init(enum am335x_gpio_modules module);

/**
 * method to setup pin multiplexing as input gpio
 *
 *@param module gpio module name
 *@param pin_nr number of the I/O pin
 *@param pin_pull I/O pull-up/-down/-none
 *@param has_to_be_debounced true if the input pin should be debounced
 */
extern void am335x_gpio_setup_pin_in(enum am335x_gpio_modules module,
                                     uint32_t pin_nr,
                                     enum am335x_gpio_pin_pull pin_pull,
                                     bool has_to_be_debounced);

/**
 * method to setup pin multiplexing as output gpio
 *
 *@param module gpio module name
 *@param pin_nr number of the I/O pin
 *@param state initial pin state (true-->1, false-->0)
 */
extern void am335x_gpio_setup_pin_out(enum am335x_gpio_modules module,
                                      uint32_t pin_nr,
                                      bool state);

/**
 * method to setup pin multiplexing as gpio
 *
 *@param module gpio module name
 *@param pin_nr number of the I/O pin
 *@param pin_dir I/O pin direction
 *@param pin_pull I/O pull-up/-down/-none
 */
extern void am335x_gpio_setup_pin(enum am335x_gpio_modules module,
                                  uint32_t pin_nr,
                                  enum am335x_gpio_pin_direction pin_dir,
                                  enum am335x_gpio_pin_pull pin_pull);

/**
 * method to set pin direction
 *
 *@param module gpio module name
 *@param pin_nr number of the I/O pin
 *@param pin_dir I/O pin direction
 */
extern void am335x_gpio_set_pin_dir(enum am335x_gpio_modules module,
                                    uint32_t pin_nr,
                                    enum am335x_gpio_pin_direction pin_dir);

/**
 * method to change state of the specified gpio pin
 *
 *@param module gpio module name
 *@param pin_nr number of the I/O pin
 *@param pin_state pin state 0/1
 */
extern void am335x_gpio_change_state(enum am335x_gpio_modules gpio_nr,
                                     uint32_t pin_nr,
                                     bool pin_state);

/**
 * method to get current state of the specified gpio pin
 *
 *@param module gpio module name
 *@param pin_nr number of the I/O pin
 *@return current pin state value
 */
extern bool am335x_gpio_get_state(enum am335x_gpio_modules gpio_nr,
                                  uint32_t pin_nr);

/**
 * method to change state of specified gpio pin set
 *
 *@param module gpio module name
 *@param pin_set set of I/O pins
 *@param pin_state pin state 0/1
 */
extern void am335x_gpio_change_states(enum am335x_gpio_modules gpio_nr,
                                      uint32_t pin_set,
                                      bool pin_state);

/**
 * method to get current state of the specified gpio pins
 *
 *@param module gpio module name
 *@return current gpio pin set state value
 */
extern uint32_t am335x_gpio_get_states(enum am335x_gpio_modules gpio_nr);

/**
 * method to enable interrupt on the specified pin
 *
 * @param module gpio module to be enabled
 * @param pin_nr pin number to be enabled
 */
extern void am335x_gpio_enable(enum am335x_gpio_modules module,
                               uint32_t pin_nr);

/**
 * method to disable interrupt on the specified pin
 *
 * @param module gpio module to be disabled
 * @param pin_nr pin number to be disabled
 */
extern void am335x_gpio_disable(enum am335x_gpio_modules module,
                                uint32_t pin_nr);

/**
 * interrupt service routine. Should be attach to INTC for processing
 */
extern void am335x_gpio_interrupt_handler(enum am335x_gpio_modules module);

/**
 * method to return the pin number having raised the interrupt
 *
 * @param module gpio module to be disabled
 * @return pin number having raised the interrupt, -1 in case of error
 */
extern int am335x_gpio_vector(enum am335x_gpio_modules module);

/**
 * method to setup pin as interrupt request line
 * the interrupt source is still disabled after configuration
 *
 * @param module gpio module name to which the ISR should be attached
 * @param pin_nr pin number to which the ISR should be attached
 * @param mode interrupt mode
 * @param has_to_be_debounced true if the input pin should be debounced
 * @param pin_pull I/O pull-up/-down/-none
 * @return execution status (0=success, -1=error)
 */
extern int am335x_gpio_setup_pin_irq(enum am335x_gpio_modules module,
                                     uint32_t pin_nr,
                                     enum am335x_gpio_interrupt_modes mode,
                                     bool has_to_be_debounced,
                                     enum am335x_gpio_pin_pull pin_pull);

#endif
