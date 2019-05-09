#pragma once
#ifndef OLED_H
#define OLED_H
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
 * Project: HEIA-FR / Embedded Systems 2 Laboratory
 *
 * Abstract: OLED-C Click Board (SPEPS114A - LCD)
 *
 * Purpose: This module provides a basic driver for the OLED-C click board.
 *          This driver is customized to run with the HEIA-FR Beaglebone
 *          Black in cape 2 (default).
 *
 * Author:  Daniel Gachet
 * Date:    05.04.2019
 */

#include <stdint.h>
#include <stdlib.h>

enum oled_versions { OLED_V100, OLED_V101 };

/**
 * each pixel is described with two uin32t_t words in big endian format, i.e.
    hword = rgb565_value >> 8
    lword = rgb565_value & 0xff
*/
struct pixel {
    uint32_t hword;  // rgb565_value >> 8
    uint32_t lword;  // rgb565_value & 0xff
};

struct pixel_b {
    uint8_t hword;  // rgb565_value >> 8
    uint8_t lword;  // rgb565_value & 0xff
};

/**
 * this method shall be called prior any other if the OLED Click Board
 * has to be connected into the cape 1. By default the cape 2 is selected
 */
extern void oled_select_cape_1();

/**
 * method to initialize the 96x96 OLED color module
 * @param version version number of the oled device
 */
extern void oled_init(enum oled_versions version);

/**
 * method to define a memory area(address) to write a display data
 *
 *        +--------+ x2/y2
 *        |  area  |
 * x1/y1  +--------+
 *
 * @param x1 coordinate
 * @param x2 coordinate
 * @param y1 coordinate
 * @param y2 coordinate
 */
extern void oled_memory_size(uint32_t x1,
                             uint32_t x2,
                             uint32_t y1,
                             uint32_t y2);

/**
 * method to send color for 1 pixel within previously defined memory area
 * @param color rgb565 pixel color
 */
extern void oled_color(uint32_t color);

/**
 * method to send an image within previously defined memory area
 * @param image image described in rgb565 pixels.
 * @param nb_pixels number of pixels of the image
 */
extern void oled_send_image(struct pixel* image, size_t nb_pixels);
extern void oled_send_image_b(struct pixel_b* image, size_t nb_pixels);

#endif
