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
 * Abstract: AM335x Pin Multiplexer Driver
 *
 * Purpose: This module implements basic services to drive the AM335x
 *          pin multiplexer module.
 *
 * Author:  Daniel Gachet
 * Date:    03.08.2016
 */

#include "am335x_mux.h"
#include <stdio.h>
#include <stdlib.h>

// pad control register bit definition
#define PAD_CONTROL_SLEWCTRL (1 << 6)
#define PAD_CONTROL_RXACTIVE (1 << 5)
#define PAD_CONTROL_PULLUP (1 << 4)
#define PAD_CONTROL_PULLUD_NONE (1 << 3)
#define PAD_CONTROL_PULLDOWN (0)
#define PAD_CONTROL_MMODE (0x7 << 0)

static const struct gpio_pad {
    volatile uint32_t* reg;
    const char* connector;
    const char* pin_name;
} gpio[4][32] =
    {
        [0] =
            {
                [0]  = {(uint32_t*)(0x44e10000 + 0x948), "none", "mdio_data"},
                [1]  = {(uint32_t*)(0x44e10000 + 0x94c), "none", "mdio_clk"},
                [2]  = {(uint32_t*)(0x44e10000 + 0x950), "p9_22", "spi0_sclk"},
                [3]  = {(uint32_t*)(0x44e10000 + 0x954), "p9_42", "spi0_d0"},
                [4]  = {(uint32_t*)(0x44e10000 + 0x958), "p9_18", "spi0_d1"},
                [5]  = {(uint32_t*)(0x44e10000 + 0x95c), "p9_17", "spi0_cs0"},
                [6]  = {(uint32_t*)(0x44e10000 + 0x960), "none", "spi0_cs1"},
                [7]  = {(uint32_t*)(0x44e10000 + 0x964),
                       "p9_17",
                       "eCAP0_in_PWM0_out"},
                [8]  = {(uint32_t*)(0x44e10000 + 0x8d0), "p8_35", "lcd_data12"},
                [9]  = {(uint32_t*)(0x44e10000 + 0x8d4), "p8_33", "lcd_data13"},
                [10] = {(uint32_t*)(0x44e10000 + 0x8d8), "p8_31", "lcd_data14"},
                [11] = {(uint32_t*)(0x44e10000 + 0x8dc), "p8_24", "lcd_data15"},
                [12] = {(uint32_t*)(0x44e10000 + 0x978), "p9_20", "uart1_ctsn"},
                [13] = {(uint32_t*)(0x44e10000 + 0x97c), "p9_19", "uart1_rtsn"},
                [14] = {(uint32_t*)(0x44e10000 + 0x980), "p9_26", "uart1_rxd"},
                [15] = {(uint32_t*)(0x44e10000 + 0x984), "p9_24", "uart1_txd"},
                [16] = {(uint32_t*)(0x44e10000 + 0x91c), "none", "mii1_txd3"},
                [17] = {(uint32_t*)(0x44e10000 + 0x920), "none", "mii1_txd2"},
                [18] = {(uint32_t*)(0x44e10000 + 0xa1c),
                        "none",
                        "usb0_drvvbus"},
                [19] = {(uint32_t*)(0x44e10000 + 0x9b0),
                        "none",
                        "xdma_event_intr0"},
                [20] = {(uint32_t*)(0x44e10000 + 0x9b4),
                        "p9_41",
                        "xdma_event_intr1"},
                [21] = {(uint32_t*)(0x44e10000 + 0x924), "none", "mii1_txd1"},
                [22] = {(uint32_t*)(0x44e10000 + 0x820), "p8_19", "gpmc_ad8"},
                [23] = {(uint32_t*)(0x44e10000 + 0x824), "p8_13", "gpmc_ad9"},
                [26] = {(uint32_t*)(0x44e10000 + 0x828), "p8_14", "gpmc_ad10"},
                [27] = {(uint32_t*)(0x44e10000 + 0x82c), "p8_17", "gpmc_ad11"},
                [28] = {(uint32_t*)(0x44e10000 + 0x928), "none", "mii1_txd0"},
                [29] = {(uint32_t*)(0x44e10000 + 0x944),
                        "none",
                        "rmii_ref_clk"},
                [30] = {(uint32_t*)(0x44e10000 + 0x870), "p9_11", "gpmc_wait0"},
                [31] = {(uint32_t*)(0x44e10000 + 0x874), "p9_13", "gpmc_wpn"},
            },
        [1] =
            {
                [0]  = {(uint32_t*)(0x44e10000 + 0x800), "p8_25", "gpmc_ad0"},
                [1]  = {(uint32_t*)(0x44e10000 + 0x804), "p8_24", "gpmc_ad1"},
                [2]  = {(uint32_t*)(0x44e10000 + 0x808), "p8_05", "gpmc_ad2"},
                [3]  = {(uint32_t*)(0x44e10000 + 0x80c), "p8_06", "gpmc_ad3"},
                [4]  = {(uint32_t*)(0x44e10000 + 0x810), "p8_23", "gpmc_ad4"},
                [5]  = {(uint32_t*)(0x44e10000 + 0x814), "p8_22", "gpmc_ad5"},
                [6]  = {(uint32_t*)(0x44e10000 + 0x818), "p8_03", "gpmc_ad6"},
                [7]  = {(uint32_t*)(0x44e10000 + 0x81c), "p8_04", "gpmc_ad7"},
                [8]  = {(uint32_t*)(0x44e10000 + 0x968), "none", "uart0_ctsn"},
                [9]  = {(uint32_t*)(0x44e10000 + 0x96c), "none", "uart0_rtsn"},
                [10] = {(uint32_t*)(0x44e10000 + 0x970), "rs_01", "uart0_rxd"},
                [11] = {(uint32_t*)(0x44e10000 + 0x974), "rs_02", "uart0_txd"},
                [12] = {(uint32_t*)(0x44e10000 + 0x830), "p8_12", "gpmc_ad12"},
                [13] = {(uint32_t*)(0x44e10000 + 0x834), "p8_11", "gpmc_ad13"},
                [14] = {(uint32_t*)(0x44e10000 + 0x838), "p8_16", "gpmc_ad14"},
                [15] = {(uint32_t*)(0x44e10000 + 0x83c), "p8_15", "gpmc_ad15"},
                [16] = {(uint32_t*)(0x44e10000 + 0x840), "p9_15", "gpmc_a0"},
                [17] = {(uint32_t*)(0x44e10000 + 0x844), "p9_23", "gpmc_a1"},
                [18] = {(uint32_t*)(0x44e10000 + 0x848), "p9_14", "gpmc_a2"},
                [19] = {(uint32_t*)(0x44e10000 + 0x84c), "p9_16", "gpmc_a3"},
                [20] = {(uint32_t*)(0x44e10000 + 0x850), "none", "gpmc_a4"},
                [21] = {(uint32_t*)(0x44e10000 + 0x854), "ul_01", "gpmc_a5"},
                [22] = {(uint32_t*)(0x44e10000 + 0x858), "ul_02", "gpmc_a6"},
                [23] = {(uint32_t*)(0x44e10000 + 0x85c), "ul_03", "gpmc_a7"},
                [24] = {(uint32_t*)(0x44e10000 + 0x860), "ul_04", "gpmc_a8"},
                [25] = {(uint32_t*)(0x44e10000 + 0x864), "none", "gpmc_a9"},
                [26] = {(uint32_t*)(0x44e10000 + 0x868), "none", "gpmc_a10"},
                [27] = {(uint32_t*)(0x44e10000 + 0x86c), "none", "gpmc_a11"},
                [28] = {(uint32_t*)(0x44e10000 + 0x878), "p9_12", "gpmc_ben1"},
                [29] = {(uint32_t*)(0x44e10000 + 0x87c), "p8_26", "gpmc_csn0"},
                [30] = {(uint32_t*)(0x44e10000 + 0x880), "p8_21", "gpmc_csn1"},
                [31] = {(uint32_t*)(0x44e10000 + 0x884), "p8_20", "gpmc_csn2"},
            },
        [2] =
            {
                [0]  = {(uint32_t*)(0x44e10000 + 0x888), "none", "gpmc_csn3"},
                [1]  = {(uint32_t*)(0x44e10000 + 0x88c), "p8_18", "gpmc_clk"},
                [2]  = {(uint32_t*)(0x44e10000 + 0x890),
                       "p8_07",
                       "gpmc_advn_ale"},
                [3]  = {(uint32_t*)(0x44e10000 + 0x894),
                       "p8_08",
                       "gpmc_oen_ren"},
                [4]  = {(uint32_t*)(0x44e10000 + 0x898), "p8_10", "gpmc_wen"},
                [5]  = {(uint32_t*)(0x44e10000 + 0x89c),
                       "p8_09",
                       "gpmc_be0n_cle"},
                [6]  = {(uint32_t*)(0x44e10000 + 0x8a0), "p8_45", "lcd_data0"},
                [7]  = {(uint32_t*)(0x44e10000 + 0x8a4), "p8_46", "lcd_data1"},
                [8]  = {(uint32_t*)(0x44e10000 + 0x8a8), "p8_43", "lcd_data2"},
                [9]  = {(uint32_t*)(0x44e10000 + 0x8ac), "p8_44", "lcd_data3"},
                [10] = {(uint32_t*)(0x44e10000 + 0x8b0), "p8_41", "lcd_data4"},
                [11] = {(uint32_t*)(0x44e10000 + 0x8b4), "p8_42", "lcd_data5"},
                [12] = {(uint32_t*)(0x44e10000 + 0x8b8), "p8_39", "lcd_data6"},
                [13] = {(uint32_t*)(0x44e10000 + 0x8bc), "p8_40", "lcd_data7"},
                [14] = {(uint32_t*)(0x44e10000 + 0x8c0), "p8_37", "lcd_data8"},
                [15] = {(uint32_t*)(0x44e10000 + 0x8c4), "p8_38", "lcd_data9"},
                [16] = {(uint32_t*)(0x44e10000 + 0x8c8), "p8_36", "lcd_data10"},
                [17] = {(uint32_t*)(0x44e10000 + 0x8cc), "p8_34", "lcd_data11"},
                [18] = {(uint32_t*)(0x44e10000 + 0x934), "none", "mii1_rxd3"},
                [19] = {(uint32_t*)(0x44e10000 + 0x938), "none", "mii1_rxd2"},
                [20] = {(uint32_t*)(0x44e10000 + 0x93c), "none", "mii1_rxd1"},
                [21] = {(uint32_t*)(0x44e10000 + 0x940), "none", "mii1_rxd0"},
                [22] = {(uint32_t*)(0x44e10000 + 0x8e0), "p8_27", "lcd_vsync"},
                [23] = {(uint32_t*)(0x44e10000 + 0x8e4), "p8_29", "lcd_hsync"},
                [24] = {(uint32_t*)(0x44e10000 + 0x8e8), "p8_28", "lcd_pclk"},
                [25] = {(uint32_t*)(0x44e10000 + 0x8ec),
                        "p8_30",
                        "lcd_ac_bias_en"},
                [26] = {(uint32_t*)(0x44e10000 + 0x8f0), "none", "mmc0_dat3"},
                [27] = {(uint32_t*)(0x44e10000 + 0x8f4), "none", "mmc0_dat2"},
                [28] = {(uint32_t*)(0x44e10000 + 0x8f8), "none", "mmc0_dat1"},
                [29] = {(uint32_t*)(0x44e10000 + 0x8fc), "none", "mmco_dat0"},
                [30] = {(uint32_t*)(0x44e10000 + 0x900), "none", "mmc0_clk"},
                [31] = {(uint32_t*)(0x44e10000 + 0x904), "none", "mmc0_cmd"},
            },
        [3] =
            {
                [0]  = {(uint32_t*)(0x44e10000 + 0x908), "none", "mii1_col"},
                [1]  = {(uint32_t*)(0x44e10000 + 0x90c), "none", "mii1_crs"},
                [2]  = {(uint32_t*)(0x44e10000 + 0x910), "none", "mii1_rx_err"},
                [3]  = {(uint32_t*)(0x44e10000 + 0x914), "none", "mii1_tx_en"},
                [4]  = {(uint32_t*)(0x44e10000 + 0x918), "none", "mii1_rx_dv"},
                [5]  = {(uint32_t*)(0x44e10000 + 0x988), "none", "i2c0_sda"},
                [6]  = {(uint32_t*)(0x44e10000 + 0x98c), "none", "i2c0_scl"},
                [7]  = {(uint32_t*)(0x44e10000 + 0x9e4), "none", "emu0"},
                [8]  = {(uint32_t*)(0x44e10000 + 0x9e8), "none", "emu1"},
                [9]  = {(uint32_t*)(0x44e10000 + 0x92c), "none", "mii1_tx_clk"},
                [10] = {(uint32_t*)(0x44e10000 + 0x930), "none", "mii1_rx_clk"},
                [13] = {(uint32_t*)(0x44e10000 + 0xa34),
                        "none",
                        "usb1_drvvbus"},
                [14] = {(uint32_t*)(0x44e10000 + 0x990),
                        "p9_31",
                        "mcasp0_aclkx"},
                [15] = {(uint32_t*)(0x44e10000 + 0x994), "p9_29", "mcasp0_fsx"},
                [16] = {(uint32_t*)(0x44e10000 + 0x998),
                        "p9_30",
                        "mcasp0_axr0"},
                [17] = {(uint32_t*)(0x44e10000 + 0x99c),
                        "p9_28",
                        "mcasp0_ahclkr"},
                [18] = {(uint32_t*)(0x44e10000 + 0x9a0),
                        "none",
                        "mcasp0_aclkr"},
                [19] = {(uint32_t*)(0x44e10000 + 0x9a4), "p9_27", "mcasp0_fsr"},
                [20] = {(uint32_t*)(0x44e10000 + 0x9a8), "none", "mcasp0_axr1"},
                [21] = {(uint32_t*)(0x44e10000 + 0x9ac),
                        "p9_25",
                        "mcasp0_ahclkx"},
            },
};

// =====================================================
// pad structures definitions
// =====================================================

typedef struct pad {
    uint32_t module;
    uint32_t pin;
    uint32_t mode;
} pad_t;

// uart pad control
static const struct uart_pad_ctrl {
    pad_t rxd;
    pad_t txd;
} uart_pad[6] = {
    [0] =
        {
            .rxd = {1, 10, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .txd = {1, 11, 0 | PAD_CONTROL_PULLUP | PAD_CONTROL_SLEWCTRL},
        },
    [4] =
        {
            .rxd = {0, 30, 6 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .txd = {0, 31, 6 | PAD_CONTROL_PULLDOWN | PAD_CONTROL_SLEWCTRL},
        },
    [5] =
        {
            .rxd = {2, 15, 4 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .txd = {2, 14, 4 | PAD_CONTROL_PULLDOWN | PAD_CONTROL_SLEWCTRL},
        },
};

// i2c pad control
static const struct i2c_pad_ctrl {
    pad_t sda;
    pad_t scl;
} i2c_pad[3] = {
    [0] =
        {
            .sda = {3,
                    16,
                    0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP |
                        PAD_CONTROL_SLEWCTRL},
            .scl = {3,
                    17,
                    0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP |
                        PAD_CONTROL_SLEWCTRL},
        },
    [1] =
        {
            /* no more supported on HEIA-FR board due to the extension board...
            .sda = {0,  4, 2 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP |
        PAD_CONTROL_SLEWCTRL}, .scl = {0,  5, 2 | PAD_CONTROL_RXACTIVE |
        PAD_CONTROL_PULLUP | PAD_CONTROL_SLEWCTRL},
        },
        [2] = {
    */
            .sda = {0,
                    12,
                    3 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP |
                        PAD_CONTROL_SLEWCTRL},
            .scl = {0,
                    3,
                    2 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP |
                        PAD_CONTROL_SLEWCTRL},
        },
};

/* ------------------------------------------------------------------------------------------
 */

// spi pad control
static const struct spi_pad_ctrl {
    pad_t sclk;
    pad_t d0;  // miso
    pad_t d1;  // mosi
    pad_t cs0;
    pad_t cs1;
} spi_pad[3] = {
    [1] =
        {
            .sclk = {3, 14, 3 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLDOWN},
            .d0   = {3, 15, 3 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .d1   = {3, 16, 3 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .cs0  = {3, 17, 3 | 0 | PAD_CONTROL_PULLUP},
            .cs1  = {0, 13, 4 | 0 | PAD_CONTROL_PULLUP},
        },
};

/* ------------------------------------------------------------------------------------------
 */

// mmc pad control
static const struct mmc_pad_ctrl {
    pad_t cmd;     // inout
    pad_t clk;     // inout (see datasheet p.4113)
    pad_t pow;     // power pin only available on controller 0 - out
                   //   pad_t sdcd; // SD-Card carrier detect
    int lines;     // number data lines
    pad_t dat[8];  // inout
} mmc_pad[3] = {
    [0] =
        {
            .cmd = {2, 31, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .clk = {2, 30, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .pow = {0, 29, 5 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            //      .sdcd   = {0,  6, 5 | PAD_CONTROL_RXACTIVE |
            //      PAD_CONTROL_PULLUP},
            .lines  = 4,
            .dat[0] = {2, 29, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[1] = {2, 28, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[2] = {2, 27, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[3] = {2, 26, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
        },
    [1] =
        {
            .cmd    = {1, 31, 2 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .clk    = {1, 30, 2 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .lines  = 8,
            .dat[0] = {1, 0, 1 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[1] = {1, 1, 1 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[2] = {1, 2, 1 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[3] = {1, 3, 1 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[4] = {1, 4, 1 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[5] = {1, 5, 1 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[6] = {1, 6, 1 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
            .dat[7] = {1, 7, 1 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
        },
};

/* ------------------------------------------------------------------------------------------
 */

// epwm pad control
static const struct epwm_pad_ctrl {
    pad_t pwma;
} epwm_pad[3] = {
    [0] =
        {
            .pwma = {1, 18, 6},
        },
    [1] =
        {
            .pwma = {2, 6, 3},
        },
};

/* ------------------------------------------------------------------------------------------
 */

static const struct cpsw_pad_ctrl {
    pad_t mdio_clk;
    pad_t mdio_data;
    pad_t mii_col;
    pad_t mii_crs;
    pad_t mii_refclk;

    pad_t mii_rx_clk;
    pad_t mii_rx_dv;
    pad_t mii_rx_err;
    pad_t mii_rxd0;
    pad_t mii_rxd1;
    pad_t mii_rxd2;
    pad_t mii_rxd3;

    pad_t mii_tx_clk;
    pad_t mii_tx_en;
    pad_t mii_txd0;
    pad_t mii_txd1;
    pad_t mii_txd2;
    pad_t mii_txd3;
} cpsw_pad = {
    .mdio_clk   = {0, 1, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
    .mdio_data  = {0, 0, 0 | PAD_CONTROL_RXACTIVE | PAD_CONTROL_PULLUP},
    .mii_crs    = {3, 1, 0 | PAD_CONTROL_RXACTIVE},
    .mii_col    = {3, 0, 0 | PAD_CONTROL_RXACTIVE},
    .mii_refclk = {0, 29, 0 | PAD_CONTROL_RXACTIVE},

    .mii_rx_clk = {3, 10, 0 | PAD_CONTROL_RXACTIVE},
    .mii_rx_dv  = {3, 4, 0 | PAD_CONTROL_RXACTIVE},
    .mii_rx_err = {3, 2, 0 | PAD_CONTROL_RXACTIVE},
    .mii_rxd0   = {2, 21, 0 | PAD_CONTROL_RXACTIVE},
    .mii_rxd1   = {2, 20, 0 | PAD_CONTROL_RXACTIVE},
    .mii_rxd2   = {2, 19, 0 | PAD_CONTROL_RXACTIVE},
    .mii_rxd3   = {2, 18, 0 | PAD_CONTROL_RXACTIVE},

    .mii_tx_clk = {3, 9, 0 | PAD_CONTROL_RXACTIVE},
    .mii_tx_en  = {3, 3, 0},
    .mii_txd0   = {0, 28, 0},
    .mii_txd1   = {0, 21, 0},
    .mii_txd2   = {0, 17, 0},
    .mii_txd3   = {0, 16, 0},
};

/* ------------------------------------------------------------------------------------------
 */

/* --------------------------------------------------------------------------
 * implementation of the private methods
 * -------------------------------------------------------------------------- */

static void pad_cfg(const pad_t* pad)
{
    volatile uint32_t* reg = gpio[pad->module][pad->pin].reg;
    *reg                   = pad->mode;
    //  *(gpio[pad->module][pad->pin].reg) = pad->mode;
}

/* --------------------------------------------------------------------------
 * implementation of the public methods
 * -------------------------------------------------------------------------- */

void am335x_mux_setup_uart_pins(enum am335x_mux_uart_modules module)
{
    const struct uart_pad_ctrl* pad = &uart_pad[module];
    pad_cfg(&pad->rxd);
    pad_cfg(&pad->txd);
}

/* -------------------------------------------------------------------------- */

void am335x_mux_setup_i2c_pins(enum am335x_mux_i2c_modules module)
{
    const struct i2c_pad_ctrl* pad = &i2c_pad[module];
    pad_cfg(&pad->sda);
    pad_cfg(&pad->scl);
}

/* -------------------------------------------------------------------------- */

void am335x_mux_setup_spi_pins(enum am335x_mux_spi_modules module)
{
    const struct spi_pad_ctrl* pad = &spi_pad[module];
    pad_cfg(&pad->sclk);
    pad_cfg(&pad->d0);
    pad_cfg(&pad->d1);
    pad_cfg(&pad->cs0);
    pad_cfg(&pad->cs1);
}

/* -------------------------------------------------------------------------- */

void am335x_mux_setup_gpio_pin(enum am335x_mux_gpio_modules module,
                               uint32_t pin_nr,
                               enum am335x_mux_gpio_pin_direction pin_dir,
                               enum am335x_mux_gpio_pin_pull pin_pull)
{
    uint32_t cfg = PAD_CONTROL_SLEWCTRL | 0x7;
    if (pin_dir == AM335X_MUX_PIN_IN) cfg |= PAD_CONTROL_RXACTIVE;
    if (pin_pull == AM335X_MUX_PULL_NONE) cfg |= PAD_CONTROL_PULLUD_NONE;
    if (pin_pull == AM335x_MUX_PULL_UP) cfg |= PAD_CONTROL_PULLUP;

    *(gpio[module][pin_nr].reg) = cfg;
}

/* -------------------------------------------------------------------------- */

void am335x_mux_setup_mmc_pins(enum am335x_mux_mmc_modules module)
{
    const struct mmc_pad_ctrl* pad = &mmc_pad[module];

    pad_cfg(&pad->clk);
    pad_cfg(&pad->cmd);

    for (int i = 0; i < pad->lines; i++) pad_cfg(&pad->dat[i]);

    if (module == AM335X_MUX_MMC0) {
        //      pad_cfg (&pad->sdcd);
        pad_cfg(&pad->pow);
    }
}

/* -------------------------------------------------------------------------- */

void am335x_mux_setup_epwm_pins(enum am335x_mux_epwm_modules module)
{
    const struct epwm_pad_ctrl* pad = &epwm_pad[module];

    pad_cfg(&pad->pwma);
}

/* -------------------------------------------------------------------------- */

void am335x_mux_setup_cpsw_pins(void)
{
    const struct cpsw_pad_ctrl* pad = &cpsw_pad;

    pad_cfg(&pad->mdio_clk);
    pad_cfg(&pad->mdio_data);
    pad_cfg(&pad->mii_crs);
    pad_cfg(&pad->mii_col);
    pad_cfg(&pad->mii_refclk);

    pad_cfg(&pad->mii_rx_clk);
    pad_cfg(&pad->mii_rx_dv);
    pad_cfg(&pad->mii_rx_err);
    pad_cfg(&pad->mii_rxd0);
    pad_cfg(&pad->mii_rxd1);
    pad_cfg(&pad->mii_rxd2);
    pad_cfg(&pad->mii_rxd3);

    pad_cfg(&pad->mii_tx_clk);
    pad_cfg(&pad->mii_tx_en);
    pad_cfg(&pad->mii_txd0);
    pad_cfg(&pad->mii_txd1);
    pad_cfg(&pad->mii_txd2);
    pad_cfg(&pad->mii_txd3);
}
