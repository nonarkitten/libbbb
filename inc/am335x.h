/*
 * am335x.h
 *
 *  Created on: Jul. 2, 2021
 *      Author: renee.cousins
 */

#ifndef LIBBBB_INC_AM335X_H_
#define LIBBBB_INC_AM335X_H_

//#if __ORDER_BIG_ENDIAN__
//#define LE32(X) __builtin_bswap32 (X)
//#define LE16(X) __builtin_bswap16 (X)
//#define BE32(X) (X)
//#define BE16(X) (X)
//#else
//#define LE32(X) (X)
//#define LE16(X) (X)
//#define BE32(X) __builtin_bswap32 (X)
//#define BE16(X) __builtin_bswap16 (X)
//#endif

#include "am335x_clock.h"
#include "am335x_console.h"
#include "am335x_dmtimer1.h"
#include "am335x_epwm.h"
#include "am335x_gpio.h"
#include "am335x_gpmc.h"
#include "am335x_i2c.h"
#include "am335x_mux.h"
#include "am335x_pru.h"
#include "am335x_spi.h"
#include "am335x_uart.h"

#endif /* LIBBBB_INC_AM335X_H_ */
