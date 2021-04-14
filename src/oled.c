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
 * Project: HEIA-FR / Embedded Systems 2 Laboratory
 *
 * Abstract: SEPS114A-C Click Board (SPEPS114A - LCD)
 *
 * Purpose: This module provides a basic driver for the SEPS114A-C click board.
 *          This driver is customized to run with the HEIA-FR Beaglebone
 *          Black in cape 2 (default).
 *
 * Origin:  This driver has been largely inspired from the mikroElektronika
 *          device driver released for the STM32F10VC MCU.
 *
 * Author:  Daniel Gachet
 * Date:    23.11.2019
 */

#include "am335x_gpio.h"
#include <stdbool.h>
#include <stdio.h>
#include "am335x_spi.h"
#include "am335x_dmtimer1.h"
#include <oled.h>

static enum oled_devices {
    SSD1351,
    SEPS114A
} oled_device = SSD1351;

enum capes
{
    CAPE_1,
    CAPE_2,
};

struct pin
{
    enum am335x_gpio_modules gpio;
    uint32_t pin;
};
static struct spi
{
    struct pin en;
    struct pin rw;
    struct pin rst;
    struct pin dc;

    enum am335x_spi_controllers ctrl;
    enum am335x_spi_channels chan;
} spi[] = {
    [CAPE_1] =
        {
            .en =
                {
                    .gpio = AM335X_GPIO0,
                    .pin = 9,
                },
            .rw =
                {
                    .gpio = AM335X_GPIO0,
                    .pin = 8,
                },
            .rst =
                {
                    .gpio = AM335X_GPIO0,
                    .pin = 10,
                },
            .dc =
                {
                    .gpio = AM335X_GPIO1,
                    .pin = 18,
                },
            .ctrl = AM335X_SPI1,
            .chan = AM335X_CHAN0,
        },
    [CAPE_2] =
        {
            .en =
                {
                    .gpio = AM335X_GPIO2,
                    .pin = 17,
                },
            .rw =
                {
                    .gpio = AM335X_GPIO2,
                    .pin = 16,
                },
            .rst =
                {
                    .gpio = AM335X_GPIO0,
                    .pin = 11,
                },
            .dc =
                {
                    .gpio = AM335X_GPIO2,
                    .pin = 6,
                },
            .ctrl = AM335X_SPI1,
            .chan = AM335X_CHAN1,
        },
};
static enum capes cape = CAPE_2;

// ----------------------------------------------------------------------------

static inline void spi_init(uint32_t freq)
{
    am335x_spi_init(spi[cape].ctrl, spi[cape].chan, freq, 8);
}

static inline void send_data(uint8_t *val, int nb)
{
    am335x_spi_write_b(spi[cape].ctrl, spi[cape].chan, val, nb);
}

static inline void send_cmd(uint8_t cmd)
{
    // select command
    am335x_gpio_change_state(spi[cape].dc.gpio, spi[cape].dc.pin, false);
    send_data(&cmd, 1);
    am335x_gpio_change_state(spi[cape].dc.gpio, spi[cape].dc.pin, true);
}

static inline void send_cmd_val(uint8_t cmd, uint8_t value)
{
    send_cmd(cmd);
    send_data(&value, 1);
}

static inline void send_cmd_val2(uint8_t cmd, uint8_t val1, uint8_t val2)
{
    send_cmd(cmd);
    uint8_t value[2] = {val1, val2};
    send_data(value, 2);
}

static inline void send_cmd_val3(uint8_t cmd, 
                                 uint8_t val1, 
                                 uint8_t val2, 
                                 uint8_t val3)
{
    send_cmd(cmd);
    uint8_t value[3] = {val1, val2, val3};
    send_data(value, 3);
}

// ----------------------------------------------------------------------------

// SEPS114A commands, for details please read SPES114A datasheet
enum SEPS114A_vals
{
    SEPS114A_SOFT_RESET = 0x01,
    SEPS114A_DISPLAY_ON_OFF = 0x02,
    SEPS114A_ANALOG_CONTROL = 0x0F,
    SEPS114A_STANDBY_ON_OFF = 0x14,
    SEPS114A_OSC_ADJUST = 0x1A,
    SEPS114A_ROW_SCAN_DIRECTION = 0x09,
    SEPS114A_DISPLAY_X1 = 0x30,
    SEPS114A_DISPLAY_X2 = 0x31,
    SEPS114A_DISPLAY_Y1 = 0x32,
    SEPS114A_DISPLAY_Y2 = 0x33,
    SEPS114A_DISPLAYSTART_X = 0x38,
    SEPS114A_DISPLAYSTART_Y = 0x39,
    SEPS114A_CPU_IF = 0x0D,
    SEPS114A_MEM_X1 = 0x34,
    SEPS114A_MEM_X2 = 0x35,
    SEPS114A_MEM_Y1 = 0x36,
    SEPS114A_MEM_Y2 = 0x37,
    SEPS114A_MEMORY_WRITE_READ = 0x1D,
    SEPS114A_DDRAM_DATA_ACCESS_PORT = 0x08,
    SEPS114A_DISCHARGE_TIME = 0x18,
    SEPS114A_PEAK_PULSE_DELAY = 0x16,
    SEPS114A_PEAK_PULSE_WIDTH_R = 0x3A,
    SEPS114A_PEAK_PULSE_WIDTH_G = 0x3B,
    SEPS114A_PEAK_PULSE_WIDTH_B = 0x3C,
    SEPS114A_PRECHARGE_CURRENT_R = 0x3D,
    SEPS114A_PRECHARGE_CURRENT_G = 0x3E,
    SEPS114A_PRECHARGE_CURRENT_B = 0x3F,
    SEPS114A_COLUMN_CURRENT_R = 0x40,
    SEPS114A_COLUMN_CURRENT_G = 0x41,
    SEPS114A_COLUMN_CURRENT_B = 0x42,
    SEPS114A_ROW_OVERLAP = 0x48,
    SEPS114A_SCAN_OFF_LEVEL = 0x49,
    SEPS114A_ROW_SCAN_ON_OFF = 0x17,
    SEPS114A_ROW_SCAN_MODE = 0x13,
    SEPS114A_SCREEN_SAVER_CONTROL = 0xD0,
    SEPS114A_SS_SLEEP_TIMER = 0xD1,
    SEPS114A_SCREEN_SAVER_MODE = 0xD2,
    SEPS114A_SS_UPDATE_TIMER = 0xD3,
    SEPS114A_RGB_IF = 0xE0,
    SEPS114A_RGB_POL = 0xE1,
    SEPS114A_DISPLAY_MODE_CONTROL = 0xE5,
};

// set memory area(address) to write a display data
void seps114a_set_area(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2)
{
    send_cmd_val(SEPS114A_MEM_X1, x1);
    send_cmd_val(SEPS114A_MEM_X2, x2);
    send_cmd_val(SEPS114A_MEM_Y1, y1);
    send_cmd_val(SEPS114A_MEM_Y2, y2);

    // set DDRAM access port
    uint8_t data[1] = {0x08};
    am335x_gpio_change_state(spi[cape].dc.gpio, spi[cape].dc.pin, false);
    send_data(data, 1);

    // switch to data mode
    am335x_gpio_change_state(spi[cape].dc.gpio, spi[cape].dc.pin, true);
}

static void seps114a_init()
{
    // soft reset
    send_cmd_val(SEPS114A_SOFT_RESET, 0x00);

    // standby ON/OFF (wait for 5ms between on/off (min delay 1ms)
    send_cmd_val(SEPS114A_STANDBY_ON_OFF, 0x01); // on
    am335x_dmtimer1_wait_ms(5);
    send_cmd_val(SEPS114A_STANDBY_ON_OFF, 0x00); // off
    am335x_dmtimer1_wait_ms(5);

    // display turn off
    send_cmd_val(SEPS114A_DISPLAY_ON_OFF, 0x00);

    // Set Oscillator operation (using external resistor and internal OSC)
    send_cmd_val(SEPS114A_ANALOG_CONTROL, 0x00);

    // set frame rate (95Hz)
    send_cmd_val(SEPS114A_OSC_ADJUST, 0x03);

    // set active display area of panel
    send_cmd_val(SEPS114A_DISPLAY_X1, 0x00);
    send_cmd_val(SEPS114A_DISPLAY_X2, 0x5F);
    send_cmd_val(SEPS114A_DISPLAY_Y1, 0x00);
    send_cmd_val(SEPS114A_DISPLAY_Y2, 0x5F);

    // Select the RGB data format and set the initial state of RGB
    // interface port (RGB 8bit interface)
    send_cmd_val(SEPS114A_RGB_IF, 0x00);

    // set RGB polarity
    send_cmd_val(SEPS114A_RGB_POL, 0x00);

    // set display mode control (SWAP:BGR, Reduce current : Normal, DC[1:0] :
    // Normal)
    send_cmd_val(SEPS114A_DISPLAY_MODE_CONTROL, 0x80);

    // set MCU Interface (MPU External interface mode, 8bits)
    send_cmd_val(SEPS114A_CPU_IF, 0x00);

    // set Memory Read/Write mode
    send_cmd_val(SEPS114A_MEMORY_WRITE_READ, 0x00);

    // set row scan direction (column: 0 --> max, row : 0 --> max)
    send_cmd_val(SEPS114A_ROW_SCAN_DIRECTION, 0x00);

    // Set row scan mode (alternate scan mode)
    send_cmd_val(SEPS114A_ROW_SCAN_MODE, 0x00);

    // set column current
    send_cmd_val(SEPS114A_COLUMN_CURRENT_R, 0x6E);
    send_cmd_val(SEPS114A_COLUMN_CURRENT_G, 0x4F);
    send_cmd_val(SEPS114A_COLUMN_CURRENT_B, 0x77);

    // set row overlap (band gap only)
    send_cmd_val(SEPS114A_ROW_OVERLAP, 0x00);

    // set discharge time (normal discharge time)
    send_cmd_val(SEPS114A_DISCHARGE_TIME, 0x01);

    // set peak pulse delay
    send_cmd_val(SEPS114A_PEAK_PULSE_DELAY, 0x00);

    // set peak pulse width
    send_cmd_val(SEPS114A_PEAK_PULSE_WIDTH_R, 0x02);
    send_cmd_val(SEPS114A_PEAK_PULSE_WIDTH_G, 0x02);
    send_cmd_val(SEPS114A_PEAK_PULSE_WIDTH_B, 0x02);

    // set precharge current
    send_cmd_val(SEPS114A_PRECHARGE_CURRENT_R, 0x14);
    send_cmd_val(SEPS114A_PRECHARGE_CURRENT_G, 0x50);
    send_cmd_val(SEPS114A_PRECHARGE_CURRENT_B, 0x19);

    // Set row scan on/off (normal row scan)
    send_cmd_val(SEPS114A_ROW_SCAN_ON_OFF, 0x00);

    // set scan off level (VCC_C*0.75)
    send_cmd_val(SEPS114A_SCAN_OFF_LEVEL, 0x04);

    // set memory access point
    send_cmd_val(SEPS114A_DISPLAYSTART_X, 0x00);
    send_cmd_val(SEPS114A_DISPLAYSTART_Y, 0x00);

    // Clear screen
    oled_memory_size(0, 95, 0, 95);
    for (int i = 0; i < 96 * 96; i++)
        oled_color(0x0000);

    // display turn on
    send_cmd_val(SEPS114A_DISPLAY_ON_OFF, 0x01);
}

// -----------------------------------------------------------------------------

// Font Direction
#define SSD1351_FO_HORIZONTAL 0x00
#define SSD1351_FO_VERTICAL 0x01
#define SSD1351_FO_VERTICAL_COLUMN 0x02

// OLED REMAMP SET
#define SSD1351_RMP_INC_HOR 0x00
#define SSD1351_RMP_INC_VER 0x01
#define SSD1351_RMP_COLOR_NOR 0x00
#define SSD1351_RMP_COLOR_REV 0x02
#define SSD1351_RMP_SEQ_RGB 0x00
#define SSD1351_RMP_SEQ_BGR 0x04
#define SSD1351_RMP_SCAN_NOR 0x00
#define SSD1351_RMP_SCAN_REV 0x10
#define SSD1351_RMP_SPLIT_DISABLE 0x00
#define SSD1351_RMP_SPLIT_ENABLE 0x20
#define SSD1351_COLOR_65K 0x00
#define SSD1351_COLOR_262K 0x80
#define SSD1351_IMG_HEAD 0x06

// Device Properties
#define SSD1351_SCREEN_WIDTH 0x60
#define SSD1351_SCREEN_HEIGHT 0x60
#define SSD1351_SCREEN_SIZE 0x2400
#define SSD1351_ROW_OFF 0x00
#define SSD1351_COL_OFF 0x10

// SSD1355 Commands
#define SSD1351_SET_COL_ADDRESS 0x15
#define SSD1351_SET_ROW_ADDRESS 0x75
#define SSD1351_WRITE_RAM 0x5C
#define SSD1351_READ_RAM 0x5D
#define SSD1351_SET_REMAP 0xA0
#define SSD1351_SET_START_LINE 0xA1
#define SSD1351_SET_OFFSET 0xA2
#define SSD1351_MODE_OFF 0xA4
#define SSD1351_MODE_ON 0xA5
#define SSD1351_MODE_NORMAL 0xA6
#define SSD1351_MODE_INVERSE 0xA7
#define SSD1351_FUNCTION 0xAB
#define SSD1351_SLEEP_ON 0xAE
#define SSD1351_SLEEP_OFF 0xAF
#define SSD1351_NOP 0xB0
#define SSD1351_SET_RESET_PRECH 0xB1
#define SSD1351_ENHANCEMENT 0xB2
#define SSD1351_CLOCK_DIV 0xB3
#define SSD1351_VSL 0xB4
#define SSD1351_GPIO 0xB5
#define SSD1351_SETSEC_PRECH 0xB6
#define SSD1351_GREY_SCALE 0xB8
#define SSD1351_LUT 0xB9
#define SSD1351_PRECH_VOL 0xBB
#define SSD1351_VCOMH 0xBE
#define SSD1351_CONTRAST 0xC1
#define SSD1351_MASTER_CONTRAST 0xC7
#define SSD1351_MUX_RATIO 0xCA
#define SSD1351_COMMAND_LOCK 0xFD
#define SSD1351_SCROLL_HOR 0x96
#define SSD1351_START_MOV 0x9E
#define SSD1351_STOP_MOV 0x9F

#define SSD1351_DEFAULT_MUX_RATIO 95
#define SSD1351_DEFAULT_START_LINE 0x80
#define SSD1351_DEFAULT_OFFSET 0x20

#define SSD1351_DEFAULT_OLED_LOCK 0x12
#define SSD1351_DEFAULT_CMD_LOCK 0xB1
#define SSD1351_DEFAULT_DIVSET 0xF1
#define SSD1351_DEFAULT_PRECHARGE 0x32
#define SSD1351_DEFAULT_VCOMH 0x05
#define SSD1351_DEFAULT_MASTER_CONT 0xCF
#define SSD1351_DEFAULT_PRECHARGE_2 0x01

#define SSD1351_DEFAULT_REMAP (SSD1351_RMP_INC_HOR | SSD1351_RMP_COLOR_NOR | \
                               SSD1351_RMP_SEQ_RGB | SSD1351_RMP_SCAN_REV |  \
                               SSD1351_RMP_SPLIT_ENABLE | SSD1351_COLOR_65K)

static inline void ssd1351_dat(uint8_t value)
{
    send_data(&value, 1);
}

static void ssd1351_set_area(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2)
{
    send_cmd(SSD1351_WRITE_RAM);
    send_cmd_val2(SSD1351_SET_COL_ADDRESS, x1 + 16, x2 + 16);
    send_cmd_val2(SSD1351_SET_ROW_ADDRESS, y1, y2);
    send_cmd(SSD1351_WRITE_RAM);
}

static void ssd1351_init()
{
    send_cmd_val(SSD1351_COMMAND_LOCK, SSD1351_DEFAULT_OLED_LOCK);
    send_cmd_val(SSD1351_COMMAND_LOCK, SSD1351_DEFAULT_CMD_LOCK);
    send_cmd(SSD1351_SLEEP_ON);
    send_cmd_val(SSD1351_SET_REMAP, SSD1351_DEFAULT_REMAP);
    send_cmd_val(SSD1351_MUX_RATIO, SSD1351_DEFAULT_MUX_RATIO);
    send_cmd_val(SSD1351_SET_START_LINE, SSD1351_DEFAULT_START_LINE);
    send_cmd_val(SSD1351_SET_OFFSET, SSD1351_DEFAULT_OFFSET);
    send_cmd_val(SSD1351_VCOMH, SSD1351_DEFAULT_VCOMH);
    send_cmd_val(SSD1351_CLOCK_DIV, SSD1351_DEFAULT_DIVSET);
    send_cmd_val(SSD1351_SET_RESET_PRECH, SSD1351_DEFAULT_PRECHARGE);
    send_cmd_val(SSD1351_SETSEC_PRECH, SSD1351_DEFAULT_PRECHARGE_2);
    send_cmd_val(SSD1351_MASTER_CONTRAST, SSD1351_DEFAULT_MASTER_CONT);
    send_cmd_val3(SSD1351_CONTRAST, 0x8a, 0x51, 0x8a);
    send_cmd_val3(SSD1351_VSL, 0xa0, 0xb5, 0x55);

    send_cmd(SSD1351_MODE_NORMAL);
    //send_cmd(SSD1351_SLEEP_ON);

    // Clear screen
    oled_memory_size(0, 95, 0, 95);
    for (int i = 0; i < 96 * 96; i++)
        oled_color(0x0000);

    // Turn the display on
    send_cmd(SSD1351_SLEEP_OFF);
}

// --- public methods ----

void oled_select_cape_1() { cape = CAPE_1; }

// init sequence for 96x96 SEPS114A color module
void oled_init2(enum oled_versions version)
{
    am335x_dmtimer1_init();

    // init gpio interface
    am335x_gpio_init(spi[cape].en.gpio);
    am335x_gpio_init(spi[cape].rw.gpio);
    am335x_gpio_init(spi[cape].rst.gpio);
    am335x_gpio_init(spi[cape].dc.gpio);
#if 0
    am335x_gpio_setup_pin_in (AM335X_GPIO0, 30, AM335X_GPIO_PULL_NONE, false);
    am335x_gpio_setup_pin_in (AM335X_GPIO0, 31, AM335X_GPIO_PULL_NONE, false);
    bool tx   = am335x_gpio_get_state (AM335X_GPIO0,  31);
    bool rx   = am335x_gpio_get_state (AM335X_GPIO0,  30);
    printf ("\ntx=%d, rx=%d\n", tx, rx);
#endif
    am335x_gpio_setup_pin_out(spi[cape].rw.gpio, spi[cape].rw.pin, false);
    am335x_gpio_setup_pin_out(spi[cape].rst.gpio, spi[cape].rst.pin, true);
    am335x_gpio_setup_pin_out(spi[cape].dc.gpio, spi[cape].dc.pin, false);

    // Hard reset
    am335x_dmtimer1_wait_ms(40);
    am335x_gpio_change_state(spi[cape].rst.gpio, spi[cape].rst.pin, false);
    am335x_dmtimer1_wait_ms(40);
    am335x_gpio_change_state(spi[cape].rst.gpio, spi[cape].rst.pin, true);
    am335x_gpio_setup_pin_out(spi[cape].en.gpio, spi[cape].en.pin, true);
    am335x_dmtimer1_wait_ms(300);

    if (version == OLED_V100)
    {
        oled_device = SEPS114A;
        spi_init(20000000);
        seps114a_init();
    }
    else
    {
        oled_device = SSD1351;
        spi_init(5000000);
        ssd1351_init();
    }
}

// set memory area(address) to write a display data
void oled_memory_size(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2)
{
    if (oled_device == SSD1351)
    {
        ssd1351_set_area(x1, x2, y1, y2);
    }
    else
    {
        seps114a_set_area(x1, x2, y1, y2);
    }
}

// send color for 1 pixel
void oled_color(uint32_t color)
{
    uint8_t data[2] = {color >> 8, color & 0xff};
    send_data(data, 2);
}

// send image
void oled_send_image(struct pixel *image, size_t nb_pixels)
{
    send_data(&image[0].hword, nb_pixels * 2);
}
