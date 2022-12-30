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
 * Project: HEIA-FR / Embedded Systems 1 Laboratory
 *
 * Abstract: AM335x PWMSS - ePWM1 and ePWM2
 *
 * Purpose: This module implements basic services to drive the AM335x
 *          Enhanced Pulse-Width Modulation module 1 and 2 (ePWM1 /eWPM2).
 *
 * Author:  Daniel Gachet
 * Date:    23.11.2019
 */

// Endian unsafe
#if 0

#include "am335x_clock.h"
#include "am335x_mux.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// TODO make big endian when used

#include "am335x_epwm.h"

// define am335x epwm controller registers
struct epwm_ctrl {
    // Time-Base Registers (TB)
    uint16_t tbctl;    // 00
    uint16_t tbsts;    // 02
    uint16_t tbphshr;  // 04
    uint16_t tbphs;    // 06
    uint16_t tbcnt;    // 08
    uint16_t tbprd;    // 0a
    uint16_t res1[1];  // 0c

    // Counter Compare Registers (CC)
    uint16_t cmpctl;  // 0e
    uint16_t cmpahr;  // 10
    uint16_t cmpa;    // 12
    uint16_t cmpb;    // 14

    // Action Qualifier Registers (AQ)
    uint16_t aqctla;   // 16
    uint16_t aqctlb;   // 18
    uint16_t aqsfrc;   // 1a
    uint16_t aqcsfrc;  // 1c

    // Dead Band Registers (DB)
    uint16_t dbctl;  // 1e
    uint16_t dbred;  // 20
    uint16_t dbfed;  // 22

    // Trip-Zone Registers (TZ)
    uint16_t tzsel;    // 24
    uint16_t res2[1];  // 26
    uint16_t tzctl;    // 28
    uint16_t tzeint;   // 2a
    uint16_t tzflg;    // 2c
    uint16_t tzclr;    // 2e
    uint16_t tzfrc;    // 30

    // Event Trigger and Interrupt Registers (ET)
    uint16_t etsel;  // 32
    uint16_t etps;   // 34
    uint16_t etflg;  // 36
    uint16_t etclr;  // 38
    uint16_t etfrc;  // 3a

    // PWM-Chopper Register (PC)
    uint16_t pcctl;  // 3c

    // reserved...
    uint16_t res3[65];

    // High Resolution Register
    uint16_t hrcnfg;  // c0
};

// define am335x pwmss controller registers
struct pwm_ctrl {
    uint32_t pwmss[64];
    uint32_t ecap[32];
    uint32_t eqep[32];
    struct epwm_ctrl epwm;
};

// system clock frequency
#define SYSCLK (100000000)

// TB register definition
#define TB_PRD_MAX (65535)
#define TB_SYNC_DISABLE (3 << 4)
#define TB_HSDIV_SHIFT (7)
#define TB_CLKDIV_SHIFT (10)

// AQ register definition
#define AQ_ZRO_SET (2 << 0)
#define AQ_ZRO_CLEAR (1 << 0)
#define AQ_CAU_CLEAR (1 << 4)

// TB divider look-up table
static const struct {
    uint32_t div;
    uint32_t clkdiv;
    uint32_t hsdiv;
} dividers[] = {
    {1, 0, 0},     //   1 *  1
    {2, 0, 1},     //   1 *  2
    {4, 0, 2},     //   1 *  4
    {6, 0, 3},     //   1 *  6
    {8, 0, 4},     //   1 *  8
    {10, 0, 5},    //   1 * 10
    {12, 0, 6},    //   1 * 12
    {14, 0, 7},    //   1 * 14
    {16, 1, 4},    //   2 *  8
    {20, 1, 5},    //   2 * 10
    {24, 1, 6},    //   2 * 12
    {28, 1, 7},    //   2 * 14
    {32, 2, 4},    //   4 *  8
    {40, 2, 5},    //   4 * 10
    {48, 2, 6},    //   4 * 12
    {56, 2, 7},    //   4 * 14
    {64, 3, 4},    //   8 *  8
    {80, 3, 5},    //   8 * 10
    {96, 3, 6},    //   8 * 12
    {112, 3, 7},   //   8 * 14
    {128, 4, 4},   //  16 *  8
    {160, 4, 5},   //  16 * 10
    {192, 4, 6},   //  16 * 12
    {224, 4, 7},   //  16 * 14
    {256, 5, 4},   //  32 *  8
    {320, 5, 5},   //  32 * 10
    {384, 5, 6},   //  32 * 12
    {448, 5, 7},   //  32 * 14
    {512, 6, 4},   //  64 *  8
    {640, 6, 5},   //  64 * 10
    {768, 6, 6},   //  64 * 12
    {896, 6, 7},   //  64 * 14
    {1024, 7, 4},  // 128 *  8
    {1280, 7, 5},  // 128 * 10
    {1536, 7, 6},  // 128 * 12
    {1792, 7, 7},  // 128 * 14
};

// am335x epwm controller memory mapped access register pointers
static volatile struct pwm_ctrl* epwm[] = {
    [AM335X_EPWM1] = (struct pwm_ctrl*)0x48302000,
    [AM335X_EPWM2] = (struct pwm_ctrl*)0x48304000,
};

// table to convert uart interface to clock module number
static const enum am335x_clock_epwm_modules epwm2clock[] = {
    [AM335X_EPWM1] = AM335X_CLOCK_EPWM1,
    [AM335X_EPWM2] = AM335X_CLOCK_EPWM2,
};

// table to convert uart interface to clock module number
static const enum am335x_mux_epwm_modules epwm2mux[] = {
    [AM335X_EPWM1] = AM335X_MUX_EPWM1,
    [AM335X_EPWM2] = AM335X_MUX_EPWM2,
};

// PWM configuration attribute
struct pwm_config {
    uint32_t freq;
    uint32_t duty;
} cfg[] = {
    [AM335X_EPWM1] = { .freq=0, .duty=0, },
    [AM335X_EPWM2] = { .freq=0, .duty=0, },
};

// macro to compute number of elements of an array
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// -----------------------------------------------------------------------------
// implementation of local methods
// -----------------------------------------------------------------------------

struct divider {
    uint32_t prd;     // period
    uint32_t clkdiv;  // clock prescale bits
    uint32_t hsdiv;   // high speed clock prescale bits
};

static struct divider get_divider(uint32_t f)
{
    uint32_t prd = (f > 0) ? (SYSCLK / f) : 0;
    uint32_t div = (prd + TB_PRD_MAX - 1) / TB_PRD_MAX;

    unsigned i = 0;
    while ((i < (ARRAY_SIZE(dividers) - 1)) && (dividers[i].div < div)) i++;

    struct divider divider = {
        .clkdiv = dividers[i].clkdiv,
        .hsdiv  = dividers[i].hsdiv,
        .prd    = (f > 0) ? (SYSCLK / dividers[i].div / f) : 0,
    };
    return divider;
}

// -----------------------------------------------------------------------------
// implementation of public methods
// -----------------------------------------------------------------------------

void am335x_epwm_init(enum am335x_epwm_controllers ctrl)
{
    volatile struct pwm_ctrl* pwm = epwm[ctrl];

    am335x_clock_enable_epwm_module(epwm2clock[ctrl]);
    am335x_mux_setup_epwm_pins(epwm2mux[ctrl]);

    pwm->epwm.tbprd  = 0;  // period set to null
    pwm->epwm.tbcnt  = 0;  // set counter to 0
    pwm->epwm.tbctl  = 0;  // set to default reset value
    pwm->epwm.cmpa   = 0;  // clear counter compare A
    pwm->epwm.cmpctl = 0;  // enable shadowing
    pwm->epwm.aqctla = 0;  // all actions disabled
}

// -----------------------------------------------------------------------------

void am335x_epwm_set_frequency(enum am335x_epwm_controllers ctrl, uint32_t freq)
{
    volatile struct pwm_ctrl* pwm = epwm[ctrl];

    if (freq > SYSCLK) return;

    // build divider attributes
    struct divider divider = get_divider(freq);

    // configure time-base registers
    pwm->epwm.tbprd = divider.prd;
    pwm->epwm.tbcnt = 0;
    pwm->epwm.tbctl = TB_SYNC_DISABLE | (divider.hsdiv << TB_HSDIV_SHIFT) |
                      (divider.clkdiv << TB_CLKDIV_SHIFT);

    // configure duty cycle
    am335x_epwm_set_duty(ctrl, cfg[ctrl].duty);

    cfg[ctrl].freq = freq;
}

// -----------------------------------------------------------------------------

void am335x_epwm_set_duty(enum am335x_epwm_controllers ctrl, uint32_t duty)
{
    volatile struct pwm_ctrl* pwm = epwm[ctrl];

    if (duty >= 100) {
        duty             = 100;
        pwm->epwm.cmpa   = 0;
        pwm->epwm.aqctla = AQ_ZRO_SET;

    } else if (duty == 0) {
        pwm->epwm.cmpa   = 0;
        pwm->epwm.aqctla = AQ_ZRO_CLEAR;

    } else {
        pwm->epwm.cmpa   = pwm->epwm.tbprd * duty / 100;
        pwm->epwm.aqctla = AQ_ZRO_SET | AQ_CAU_CLEAR;
    }
    cfg[ctrl].duty = duty;
}

#endif
