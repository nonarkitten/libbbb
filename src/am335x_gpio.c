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
 * This module is based on the software library developped by Texas Instruments
 * Incorporated - http://www.ti.com/ for its AM335x starter kit.
 *
 * Project: HEIA-FR / Embedded Systems 1+2 Laboratory
 *
 * Abstract: AM335x GPIO Driver
 *
 * Purpose: This module implements basic services to drive the AM335x
 *          GPIO module.
 *
 * Author:  Daniel Gachet
 * Date:    21.03.2016
 */

#include "am335x_gpio.h"
#include "am335x_clock.h"
#include "am335x_mux.h"

// TODO make big endian when used

// define am335x gpio registers
struct am335x_gpio_ctrl {
    uint32_t revision;        // 00
    uint32_t res1[3];         // 04-0c
    uint32_t sysconfig;       // 10
    uint32_t res2[4];         // 14-20
    uint32_t irqstatus_raw0;  // 24
    uint32_t irqstatus_raw1;  // 28
    uint32_t irqstatus0;      // 2c
    uint32_t irqstatus1;      // 30
    uint32_t irqstatus_set0;  // 34
    uint32_t irqstatus_set1;  // 38
    uint32_t irqstatus_clr0;  // 3c
    uint32_t irqstatus_clr1;  // 40
    uint32_t irqwaken0;       // 44
    uint32_t irqwaken1;       // 48
    uint32_t res3[50];        // 4c-110
    uint32_t sysstatus;       // 114
    uint32_t res4[6];         // 118-12c
    uint32_t ctrl;            // 130
    uint32_t oe;              // 134
    uint32_t datain;          // 138
    uint32_t dataout;         // 13c
    uint32_t leveldetect0;    // 140
    uint32_t leveldetect1;    // 144
    uint32_t risingdetect;    // 148
    uint32_t fallingdetect;   // 14c
    uint32_t debouncenable;   // 150
    uint32_t debouncingtime;  // 154
    uint32_t res5[14];        // 158-18c
    uint32_t cleardataout;    // 190
    uint32_t setdataout;      // 194
};

// SYSCONFIG register bit definition
#define SYSCONFIG_IDLEMODE_MASK (0x3 << 3)
#define SYSCONFIG_IDLEMODE_FORCEIDLE (0x0 << 3)
#define SYSCONFIG_IDLEMODE_NOIDLE (0x1 << 3)
#define SYSCONFIG_IDLEMODE_SMARTIDLE (0x2 << 3)
#define SYSCONFIG_IDLEMODE_SMARTIDLEWAKEUP (0x3 << 3)

#define SYSCONFIG_ENAWAKEUP (1 << 2)
#define SYSCONFIG_SOFTRESET (1 << 1)
#define SYSCONFIG_AUTOIDLE (1 << 0)

// SYSSTATUS register bit definition
#define SYSSTATUS_RESETDONE (1 << 0)

// CTRL register bit definition
#define CTRL_GATINGRATIO (0x3 << 1)
#define CTRL_GATINGRATIO_N_1 (0x0 << 1)
#define CTRL_GATINGRATIO_N_2 (0x1 << 1)
#define CTRL_GATINGRATIO_N_4 (0x2 << 1)
#define CTRL_GATINGRATIO_N_8 (0x3 << 1)

#define CTRL_DISABLEMODULE (1 << 0)

// am335x gpio module memory mapped access register pointers
static volatile struct am335x_gpio_ctrl* gpio_ctrl[] = {
    (struct am335x_gpio_ctrl*)0x44e07000,
    (struct am335x_gpio_ctrl*)0x4804c000,
    (struct am335x_gpio_ctrl*)0x481ac000,
    (struct am335x_gpio_ctrl*)0x481ae000,
};

// am335x gpio module configuration states
static bool is_initialized[4] = {false, false, false, false};

// convertion am335x_gpio to am335x_clock table
static const enum am335x_clock_gpio_modules gpio2clock[] = {
    AM335X_CLOCK_GPIO0,
    AM335X_CLOCK_GPIO1,
    AM335X_CLOCK_GPIO2,
    AM335X_CLOCK_GPIO3,
};

// convertion am335x_gpio to am335x_mux table
static const enum am335x_mux_gpio_modules gpio2mux[] = {
    AM335X_MUX_GPIO0,
    AM335X_MUX_GPIO1,
    AM335X_MUX_GPIO2,
    AM335X_MUX_GPIO3,
};

/**
 * GPIO ISR Handler Structure Definition
 */
struct gpio_isr_handlers {
    am335x_gpio_handler_t routine; /* application specific interrupt routine */
    void* param;                   /* application specific parameter */
    enum am335x_gpio_interrupt_modes mode; /* pin operation mode */
};
static struct gpio_isr_handlers handlers[AM335X_GPIO_NB_MODULES][32];

/* -- Local methods ---------------------------------------------------------*/

static void udelay(int us_delay)
{
    while (us_delay > 0) {
        us_delay--;
        __asm(
            "   mov r3, #1000/4;"
            "1:     nop;"
            "   subs r3, #1;"
            "   bne  1b;");
    }
}

/* -- Interrupt Service Routine  ------------------------------------------- */

void am335x_gpio_interrupt_handler(enum am335x_gpio_modules module)
{
    volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
    uint32_t isr                           = gpio->irqstatus0;
    gpio->irqstatus0                       = (isr);

    struct gpio_isr_handlers* handler = handlers[module];
    for (int pin = 0; pin < 32; pin++) {
        if (((isr & (1 << pin)) != 0) & (handler[pin].routine != 0)) {
            handler[pin].routine(module, pin, handler[pin].param);
        }
    }
}

/* --------------------------------------------------------------------------
 * implementation of the public methods
 * -------------------------------------------------------------------------- */

void* am335x_gpio_init(enum am335x_gpio_modules module)
{
    volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];

    if (!is_initialized[module]) {
        // enable gpio module clock
        am335x_clock_enable_gpio_module(gpio2clock[module]);

        // enable gpio module
        gpio->ctrl &= (~CTRL_DISABLEMODULE);

        // reset gpio module and wait until performed
        gpio->sysconfig |= (SYSCONFIG_SOFTRESET);
        while ((gpio->sysstatus & (SYSSTATUS_RESETDONE)) == 0)
            ;

        // mask all interrupt request lines and clear status
        gpio->irqstatus_clr0 = -1;
        gpio->irqstatus_clr1 = -1;
        gpio->risingdetect   = 0;
        gpio->fallingdetect  = 0;
        gpio->leveldetect0   = 0;
        gpio->leveldetect1   = 0;
        gpio->irqstatus0     = -1;
        gpio->irqstatus1     = -1;
        gpio->debouncingtime = (30);

        // mark as initialized
        is_initialized[module] = true;
    }

    return (void*)gpio;
}

/* -------------------------------------------------------------------------- */

void am335x_gpio_setup_pin_in(enum am335x_gpio_modules module,
                              uint32_t pin_nr,
                              enum am335x_gpio_pin_pull pin_pull,
                              bool has_to_be_debounced)
{
    volatile struct am335x_gpio_ctrl* gpio = am335x_gpio_init(module);

    // configure pin direction
    am335x_gpio_set_pin_dir(module, pin_nr, AM335X_GPIO_PIN_IN);

    if (has_to_be_debounced) {
        gpio->debouncenable |= (1 << pin_nr);
        udelay(1000);  // short delay due to debouncing
    } else {
        gpio->debouncenable &= ~(1 << pin_nr);
    }

    // configure am335x mux as gpio
    am335x_mux_setup_gpio_pin(gpio2mux[module],
                              pin_nr,
                              AM335X_MUX_PIN_IN,
                              (enum am335x_mux_gpio_pin_pull)pin_pull);
}

/* -------------------------------------------------------------------------- */

void am335x_gpio_setup_pin_out(enum am335x_gpio_modules module,
                               uint32_t pin_nr,
                               bool pin_state)
{
    // configure default value
    am335x_gpio_change_state(module, pin_nr, pin_state);

    // configure pin direction
    am335x_gpio_set_pin_dir(module, pin_nr, AM335X_GPIO_PIN_OUT);

    // configure am335x mux as gpio
    am335x_mux_setup_gpio_pin(
        gpio2mux[module], pin_nr, AM335X_MUX_PIN_IN, AM335X_MUX_PULL_NONE);
}

/* -------------------------------------------------------------------------- */

void am335x_gpio_setup_pin(enum am335x_gpio_modules module,
                           uint32_t pin_nr,
                           enum am335x_gpio_pin_direction pin_dir,
                           enum am335x_gpio_pin_pull pin_pull)
{
    // configure pin direction
    am335x_gpio_set_pin_dir(module, pin_nr, pin_dir);

    // configure am335x mux as gpio
    am335x_mux_setup_gpio_pin(gpio2mux[module],
                              pin_nr,
                              AM335X_MUX_PIN_IN,
                              (enum am335x_mux_gpio_pin_pull)pin_pull);
}

/* -------------------------------------------------------------------------- */

void am335x_gpio_set_pin_dir(enum am335x_gpio_modules module,
                             uint32_t pin_nr,
                             enum am335x_gpio_pin_direction pin_dir)
{
    volatile struct am335x_gpio_ctrl* gpio = am335x_gpio_init(module);

    // reset pin configuration
    gpio->irqstatus_clr0 = (1 << pin_nr);
    gpio->risingdetect &= ~(1 << pin_nr);
    gpio->fallingdetect &= ~(1 << pin_nr);
    gpio->leveldetect0 &= ~(1 << pin_nr);
    gpio->leveldetect1 &= ~(1 << pin_nr);
    gpio->debouncenable &= ~(1 << pin_nr);

    // configure pin direction
    if (pin_dir == AM335X_GPIO_PIN_OUT) {
        gpio->oe &= ~(1 << pin_nr);
    } else {
        gpio->oe |= (1 << pin_nr);
    }
}

/* -------------------------------------------------------------------------- */

void am335x_gpio_change_state(enum am335x_gpio_modules module,
                              uint32_t pin_nr,
                              bool pin_state)
{
    volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
    if (pin_state) {
        gpio->setdataout = (1 << pin_nr);
    } else {
        gpio->cleardataout = (1 << pin_nr);
    }
}

/* -------------------------------------------------------------------------- */

bool am335x_gpio_get_state(enum am335x_gpio_modules module, uint32_t pin_nr)
{
    volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
    return (gpio->datain & (1 << pin_nr)) != 0;
}

/* -------------------------------------------------------------------------- */

void am335x_gpio_change_states(enum am335x_gpio_modules module,
                               uint32_t pin_set,
                               bool pin_state)
{
    volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
    pin_set = (pin_set);
    if (pin_state) {
        gpio->setdataout = pin_set;
    } else {
        gpio->cleardataout = pin_set;
    }
}

/* -------------------------------------------------------------------------- */

uint32_t am335x_gpio_get_states(enum am335x_gpio_modules module)
{
    volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
    return (gpio->datain);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int am335x_gpio_attach(enum am335x_gpio_modules module,
                       uint32_t pin_nr,
                       enum am335x_gpio_interrupt_modes mode,
                       bool has_to_be_debounced,
                       am335x_gpio_handler_t routine,
                       void* param)
{
    int status = -1;

    if ((module < AM335X_GPIO_NB_MODULES) && (pin_nr < 32) &&
        (handlers[module][pin_nr].routine == 0)) {
        handlers[module][pin_nr].routine = routine;
        handlers[module][pin_nr].param   = param;
        handlers[module][pin_nr].mode    = mode;

        am335x_gpio_setup_pin_irq(
            module, pin_nr, mode, has_to_be_debounced, AM335X_GPIO_PULL_NONE);

        gpio_ctrl[module]->irqstatus_set0 = (1 << pin_nr);

        status = 0;
    }

    return status;
}

void am335x_gpio_detach(enum am335x_gpio_modules module, uint32_t pin_nr)
{
    if ((module < AM335X_GPIO_NB_MODULES) && (pin_nr < 32)) {
        volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
        gpio->irqstatus_clr0 = (1 << pin_nr);
        gpio->risingdetect &= ~(1 << pin_nr);
        gpio->fallingdetect &= ~(1 << pin_nr);
        gpio->leveldetect0 &= ~(1 << pin_nr);
        gpio->leveldetect1 &= ~(1 << pin_nr);

        handlers[module][pin_nr].routine = 0;
        handlers[module][pin_nr].param   = 0;
        handlers[module][pin_nr].mode    = 0;
    }
}

void am335x_gpio_enable(enum am335x_gpio_modules module, uint32_t pin_nr)
{
    if ((module < AM335X_GPIO_NB_MODULES) && (pin_nr < 32)) {
        volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
        gpio->irqstatus_set0 |= (1 << pin_nr);
    }
}

void am335x_gpio_disable(enum am335x_gpio_modules module, uint32_t pin_nr)
{
    if ((module < AM335X_GPIO_NB_MODULES) && (pin_nr < 32)) {
        volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
        gpio->irqstatus_clr0 |= (1 << pin_nr);
    }
}

int am335x_gpio_vector(enum am335x_gpio_modules module)
{
    volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
    uint32_t isr = (gpio->irqstatus0);

    uint32_t pin = 0;
    while (pin < 32) {
        if ((isr & (1 << pin)) != 0) break;
        pin++;
    }
    if (pin < 32) {
        gpio->irqstatus0 = (1 << pin);
        return pin;
    }
    return -1;
}

int am335x_gpio_setup_pin_irq(enum am335x_gpio_modules module,
                              uint32_t pin_nr,
                              enum am335x_gpio_interrupt_modes mode,
                              bool has_to_be_debounced,
                              enum am335x_gpio_pin_pull pin_pull)
{
    int status = -1;

    if ((module < AM335X_GPIO_NB_MODULES) && (pin_nr < 32)) {
        am335x_gpio_setup_pin_in(module, pin_nr, pin_pull, has_to_be_debounced);

        volatile struct am335x_gpio_ctrl* gpio = gpio_ctrl[module];
        switch (mode) {
            case AM335X_GPIO_IRQ_RISING:
                gpio->risingdetect |= (1 << pin_nr);
                break;
            case AM335X_GPIO_IRQ_FALLING:
                gpio->fallingdetect |= (1 << pin_nr);
                break;
            case AM335X_GPIO_IRQ_BOTH:
                gpio->fallingdetect |= (1 << pin_nr);
                gpio->risingdetect |= (1 << pin_nr);
                break;
            case AM335X_GPIO_IRQ_LOW:
                gpio->leveldetect0 |= (1 << pin_nr);
                break;
            case AM335X_GPIO_IRQ_HIGH:
                gpio->leveldetect1 |= (1 << pin_nr);
                break;
        }
        status = 0;
    }

    return status;
}
