#pragma once
#ifndef AM335X_EPWM_H
#define AM335X_EPWM_H
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
 * This module is based on the software library developped by Texas Instruments
 * Incorporated - http://www.ti.com/ for its AM335x starter kit.
 *
 * Project: HEIA-FR / Embedded Systems 1 Laboratory
 *
 * Abstract: AM335x PWMSS - ePWM1/2
 *
 * Purpose: This module implements basic services to drive the AM335x
 *          Enhanced Pulse-Width Modulation module 1 and 2 (ePWM1 /eWPM2).
 *
 * Author:  Daniel Gachet
 * Date:    23.11.2019
 */

#include <stdint.h>

/**
 * defines the available ePWM controllers of the am335x
 */
enum am335x_epwm_controllers {
    AM335X_EPWM1,
    AM335X_EPWM2,
};

/**
 * method to initialize the AM335x ePWM1 or ePWM2 resources.
 * should be called prior any other methods.
 * @param ctrl am335x epwm controller name
 */
extern void am335x_epwm_init(enum am335x_epwm_controllers ctrl);

/**
 * method to set the PWM frequency o
 * @param ctrl am335x epwm controller name
 * @param freq PWM frequency in Hz
 */
extern void am335x_epwm_set_frequency(enum am335x_epwm_controllers ctrl,
                                      uint32_t freq);

/**
 * method to define the duty cycle
 * @param ctrl am335x epwm controller name
 * @param duty duty cycle in %
 */
extern void am335x_epwm_set_duty(enum am335x_epwm_controllers ctrl,
                                 uint32_t duty);

#endif
