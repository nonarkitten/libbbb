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
 * Project: HEIA-FR / Embedded Systems 1+2 Laboratory
 *
 * Abstract: AM335x McSPI Driver
 *
 * Purpose: This module implements basic services to drive the AM335x McSPI
 *          controller.
 *
 * Author:  Daniel Gachet
 * Date:    17.03.2019
 */

#include "am335x_spi.h"

#include "am335x_clock.h"
#include "am335x_mux.h"

// define am335x spi channel registers
struct am335x_spi_channel {
    uint32_t chconf;  // 12c + (n * 0x14)
    uint32_t chstat;  // 130 + (n * 0x14)
    uint32_t chctrl;  // 134 + (n * 0x14)
    uint32_t tx;      // 138 + (n * 0x14)
    uint32_t rx;      // 13c + (n * 0x14)
};

// SPI CHxSTAT channel status register bit definition
#define CHSTAT_RXFFF (1 << 6)
#define CHSTAT_RXFFE (1 << 5)
#define CHSTAT_TXFFF (1 << 4)
#define CHSTAT_TXFFE (1 << 3)
#define CHSTAT_EOT   (1 << 2)
#define CHSTAT_TXS   (1 << 1)
#define CHSTAT_RXS   (1 << 0)

// SPI CHxCTRL channel control register bit defintion
#define CHCTRL_EN    (1 << 0)

// define am335x spi controller registers
struct am335x_spi_ctrl {
    uint32_t                  revision;   // 000
    uint32_t                  res1[67];   // 004-10c
    uint32_t                  sysconfig;  // 110
    uint32_t                  sysstatus;  // 114
    uint32_t                  irqstatus;  //  118
    uint32_t                  irqenable;  // 11c
    uint32_t                  res2[1];    // 120
    uint32_t                  syst;       // 124
    uint32_t                  modulctrl;  // 128
    struct am335x_spi_channel channel[4];
    uint32_t                  xferlevel;  //  17c
};

// SPI SYSCONFIG register bit definition
#define SYSCONFIG_CLKACTIVITY_MASK    (0x3 << 8)
#define SYSCONFIG_CLKACTIVITY_NONE    (0x0 << 8)
#define SYSCONFIG_CLKACTIVITY_OCP     (0x1 << 8)
#define SYSCONFIG_CLKACTIVITY_FUNC    (0x2 << 8)
#define SYSCONFIG_CLKACTIVITY_BOTH    (0x3 << 8)

#define SYSCONFIG_SIDLEMODE_MASK      (0x3 << 3)
#define SYSCONFIG_SIDLEMODE_FORCEIDLE (0x0 << 3)
#define SYSCONFIG_SIDLEMODE_NOIDLE    (0x1 << 3)
#define SYSCONFIG_SIDLEMODE_SMARTIDLE (0x2 << 3)
#define SYSCONFIG_SIDLEMODE_RESERVED  (0x3 << 3)

#define SYSCONFIG_SRST                (1 << 1)
#define SYSCONFIG_AUTOIDLE            (1 << 0)

// SPI SYSTATUS bit definition
#define SYSSTATUS_RDONE               (1 << 0)

// spi clocking values
#define SYSTEM_CLOCK                  48000000

// am335x uart controllers memory mapped access register pointers
static volatile struct am335x_spi_ctrl* spi_ctrl[] = {
    (struct am335x_spi_ctrl*)0x48030000,
    (struct am335x_spi_ctrl*)0x481A0000,
};

// table to convert uart interface to clock module number
static const enum am335x_clock_spi_modules spi2clock[] = {
    AM335X_CLOCK_SPI0,
    AM335X_CLOCK_SPI1,
};

// table to convert uart interface to clock module number
static const enum am335x_mux_spi_modules spi2mux[] = {
    AM335X_MUX_SPI0,
    AM335X_MUX_SPI1,
};

/* --------------------------------------------------------------------------
 * implementation of the public methods
 * -------------------------------------------------------------------------- */

void am335x_spi_init(enum am335x_spi_controllers ctrl,
                     enum am335x_spi_channels channel, uint32_t bus_speed,
                     uint32_t word_len) {
    static bool is_initialized = false;

    volatile struct am335x_spi_ctrl*    spi  = spi_ctrl[ctrl];
    volatile struct am335x_spi_channel* chan = &spi->channel[channel];

    if (!is_initialized) {
        //  enable spi module clock
        am335x_clock_enable_spi_module(spi2clock[ctrl]);

        // reset and disable spi controller
        spi->sysconfig = __builtin_bswap32(SYSCONFIG_SRST);
        while ((spi->sysstatus & __builtin_bswap32(SYSSTATUS_RDONE)) == 0)
            ;

        // configure clock activity and idle mode
        spi->sysconfig = __builtin_bswap32(SYSCONFIG_SIDLEMODE_NOIDLE |
                                           SYSCONFIG_CLKACTIVITY_BOTH);

        //  configure module contoller
        //  module is configured after the channel to avoid glitch on chip select
        //  signal
        spi->modulctrl =
            __builtin_bswap32((0 << 8)    // �fifo managed with ctrl register
                              | (0 << 7)  // multiword disabled
                              | (1 << 4)  // �initial spi delay = 4 spi clock
                              | (0 << 3)  // functional mode
                              | (0 << 2)  // �master mode
                              | (0 << 1)  // use SPIEN as chip select
                              | (1 << 0)  // multi channel mode
            );

        // set fifo level to 0
        spi->xferlevel = 0;
        is_initialized = true;
    }

    // compute frequency flags
    uint32_t clkg   = 0;
    uint32_t clkd   = 0;
    uint32_t extclk = 0;

    // calculate the frequency ratio
    uint32_t ratio = SYSTEM_CLOCK / bus_speed;

    //  if ratio is not a power of 2, set granularity of 1 clock cycle
    if ((ratio & (ratio - 1)) != 0) {
        clkg   = 1;
        clkd   = (ratio - 1) & 0xf;
        extclk = (ratio - 1) >> 4;
    } else {  // compute log2 (ratio)
        while (ratio != 1) {
            ratio /= 2;
            clkd++;
        }
    }

    // configure channel (tx & rx fifo disabled)
    chan->chconf =
        __builtin_bswap32((clkg << 29)             // clock granularity
                          | (3 << 25)              // chip select time control 2.5 cycles
                          | (0 << 21)              // spienslv = 0
                          | (0 << 20)              // force = 0
                          | (0 << 19)              // turbo = 0
                          | (1 << 16)              // TX on D1, RX on D0
                          | (0 << 14)              // DMA transfer disabled
                          | (0 << 12)              // TX + RX mode
                          | ((word_len - 1) << 7)  // spi word len
                          | (1 << 6)               // spien polarity = low
                          | (clkd << 2)            // frequency diviver
                          | (0 << 1)               // spiclk polarity = high
                          | (0 << 0)               // spiclk phase = odd
        );

    // configure clock ration extender
    chan->chctrl = __builtin_bswap32(extclk << 8);

    // setup spi pins
    am335x_mux_setup_spi_pins(spi2mux[ctrl]);
}

/* -------------------------------------------------------------------------- */

int am335x_spi_xfer(enum am335x_spi_controllers ctrl,
                    enum am335x_spi_channels channel, uint8_t* buffer,
                    size_t buffer_len) {
    volatile struct am335x_spi_ctrl*    spi  = spi_ctrl[ctrl];
    volatile struct am335x_spi_channel* chan = &spi->channel[channel];

    // enable channel
    chan->chctrl |= __builtin_bswap32(CHCTRL_EN);
    chan->chconf |= __builtin_bswap32(1 << 20);

    while (buffer_len--) {
        while ((chan->chstat & __builtin_bswap32(CHSTAT_TXS)) == 0) continue;
        chan->tx = __builtin_bswap32(*buffer);
        while ((chan->chstat & __builtin_bswap32(CHSTAT_RXS)) == 0) continue;
        *buffer++ = __builtin_bswap32(chan->rx) & 0xff;
    }

    // wait until transfer complete
    // while ((chan->chstat & CHSTAT_EOT) == 0) continue;

    // restore chconf
    chan->chconf &= ~__builtin_bswap32(1 << 20);
    // disable channel
    chan->chctrl &= ~__builtin_bswap32(CHCTRL_EN);

    return 0;
}
