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
 * Abstract: AM335x I2C Driver
 *
 * Purpose: This module implements basic services to drive the AM335x I2C
 *          controller.
 *
 * Author:  Daniel Gachet
 * Date:    03.07.2015
 */

#include "am335x_i2c.h"

#include "am335x_clock.h"
#include "am335x_mux.h"

// define am335x i2c controller registers
struct am335x_i2c_ctrl {
  uint32_t revnb_lo;         // 00
  uint32_t revnb_hi;         // 04
  uint32_t res1[2];          // 08-0c
  uint32_t sysc;             // 10
  uint32_t res2[4];          // 14-20
  uint32_t irqstatus_raw;    // 24
  uint32_t irqstatus;        // 28
  uint32_t irqenable_set;    // 2c
  uint32_t irqenable_clr;    // 30
  uint32_t we;               // 34
  uint32_t dmarxenable_set;  // 38
  uint32_t dmatxenable_set;  // 3c
  uint32_t dmarxenable_clr;  // 40
  uint32_t dmatxenable_clr;  // 44
  uint32_t dmarxwake_en;     // 48
  uint32_t dmatxwake_en;     // 4c
  uint32_t res3[16];         // 50-8c
  uint32_t syss;             // 90
  uint32_t buf;              // 94
  uint32_t cnt;              // 98
  uint32_t data;             // 9c
  uint32_t res4[1];          //  a0
  uint32_t con;              // a4
  uint32_t oa;               // a8
  uint32_t sa;               // ac
  uint32_t psc;              // b0
  uint32_t scll;             // b4
  uint32_t sclh;             // b8
  uint32_t systest;          // bc
  uint32_t bufstat;          // c0
  uint32_t oa1;              // c4
  uint32_t oa2;              // c8
  uint32_t oa3;              // cc
  uint32_t actoa;            // d0
  uint32_t sblock;           // d4
};

// I2C SYSC register bit definition
#define SYSC_CLKACTIVITY_MASK (0x3 << 8)
#define SYSC_CLKACTIVITY_NONE (0x0 << 8)
#define SYSC_CLKACTIVITY_OCP (0x1 << 8)
#define SYSC_CLKACTIVITY_FUNC (0x2 << 8)
#define SYSC_CLKACTIVITY_BOTH (0x3 << 8)

#define SYSC_IDLEMODE_MASK (0x3 << 3)
#define SYSC_IDLEMODE_FORCEIDLE (0x0 << 3)
#define SYSC_IDLEMODE_NOIDLE (0x1 << 3)
#define SYSC_IDLEMODE_SMARTIDLE (0x2 << 3)
#define SYSC_IDLEMODE_SMARTIDLE_WAKEUP (0x3 << 3)

#define SYSC_ENAWAKEUP (1 << 2)
#define SYSC_SRST (1 << 1)
#define SYSC_AUTOIDLE (1 << 0)

// I2C IRQSTATUS_RAW register bit definition
#define IRQSTATUS_RAW_XDR (1 << 14)
#define IRQSTATUS_RAW_RDR (1 << 13)
#define IRQSTATUS_RAW_BB (1 << 12)
#define IRQSTATUS_RAW_ROVR (1 << 11)
#define IRQSTATUS_RAW_XUDF (1 << 10)
#define IRQSTATUS_RAW_AAS (1 << 9)
#define IRQSTATUS_RAW_BF (1 << 8)
#define IRQSTATUS_RAW_AERR (1 << 7)
#define IRQSTATUS_RAW_STC (1 << 6)
#define IRQSTATUS_RAW_GC (1 << 5)
#define IRQSTATUS_RAW_XRDY (1 << 4)
#define IRQSTATUS_RAW_RRDY (1 << 3)
#define IRQSTATUS_RAW_ARDY (1 << 2)
#define IRQSTATUS_RAW_NACK (1 << 1)
#define IRQSTATUS_RAW_AL (1 << 0)

// I2C CON register bit definition
#define CON_I2C_EN (1 << 15)

#define CON_OPMODE_MASK (0x3 << 12)
#define CON_OPMODE_RESERVED (0x3 << 12)
#define CON_OPMODE_SCCB (0x2 << 12)
#define CON_OPMODE_HSI2C (0x1 << 12)
#define CON_OPMODE_FSI2C (0x0 << 12)

#define CON_STB (1 << 11)
#define CON_MST (1 << 10)
#define CON_TRX (1 << 9)
#define CON_XSA (1 << 8)
#define CON_XOA0 (1 << 7)
#define CON_XOA1 (1 << 6)
#define CON_XOA2 (1 << 5)
#define CON_XOA3 (1 << 4)
#define CON_STP (1 << 1)
#define CON_STT (1 << 0)

// I2C BUF register bit definition
#define BUF_RDMA_EN (1 << 15)
#define BUF_RXFIFO_CLR (1 << 14)
#define BUF_RXTRSH_MASK (0x3f << 8)
#define BUF_RXTRSH_SHIFT (8)
#define BUF_XDMA_EN (1 << 7)
#define BUF_TXFIFO_CLR (1 << 6)
#define BUF_TXTRSH_MASK (0x3f << 0)
#define BUF_TXTRSH_SHIFT (0)

// I2C BUF register bit definition
#define SYSS_RDONE (1 << 0)

// i2c clocking values
#define SYSTEM_CLOCK 48000000
#define INTERNAL_CLOCK 12000000

// am335x uart controllers memory mapped access register pointers
static volatile struct am335x_i2c_ctrl* i2c_ctrl[] = {
    (struct am335x_i2c_ctrl*)0x44e0B000,
    //  (struct am335x_i2c_ctrl*)0x4802a000,
    (struct am335x_i2c_ctrl*)0x4819c000,
};

// table to convert uart interface to clock module number
static const enum am335x_clock_i2c_modules i2c2clock[] = {
    AM335X_CLOCK_I2C0,
    //  AM335X_CLOCK_I2C1,
    AM335X_CLOCK_I2C2,
};

// table to convert uart interface to clock module number
static const enum am335x_mux_i2c_modules i2c2mux[] = {
    AM335X_MUX_I2C0,
    //  AM335X_MUX_I2C1,
    AM335X_MUX_I2C2,
};

/* --------------------------------------------------------------------------
 * implementation of local methods
 * -------------------------------------------------------------------------- */

/**
 * method to wait until specified bit is set in the irqstatus_raw register
 *
 *@param i2c i2c controller
 *@param bit bit to wait for (mask)
 *@return status information: 0 --> bit is set, -1 --> error has been detected
 */
static int wait_for_status(volatile struct am335x_i2c_ctrl* i2c, int bit) {
  int status = 0;
  int timeout = 20000;
  while (1) {
    // check for malfunction...
    status = -1;
    if ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_AL)) != 0) break;
    if ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_AERR)) != 0)
      break;
    if (timeout == 0)
      break;
    else
      timeout--;

    // check for valid bit...
    status = 0;
    if ((i2c->irqstatus_raw & __builtin_bswap32(bit)) != 0) break;
  }

  // clear error bits
  i2c->irqstatus = __builtin_bswap32(IRQSTATUS_RAW_AL | IRQSTATUS_RAW_AERR);

  // return status information
  return status;
}

/* --------------------------------------------------------------------------
 * implementation of the public methods
 * -------------------------------------------------------------------------- */

void am335x_i2c_init(enum am335x_i2c_controllers ctrl, uint32_t bus_speed) {
  volatile struct am335x_i2c_ctrl* i2c = i2c_ctrl[ctrl];

  //  enable i2c module clock
  am335x_clock_enable_i2c_module(i2c2clock[ctrl]);

  // setup i2c pins
  am335x_mux_setup_i2c_pins(i2c2mux[ctrl]);

  // reset and disable i2c controller
  i2c->sysc = __builtin_bswap32(SYSC_SRST);
  while ((i2c->syss & __builtin_bswap32(SYSS_RDONE)) != 0)
    ;
  i2c->con &= ~__builtin_bswap32(CON_I2C_EN);

  // configure clock activity and idle mode
  i2c->sysc = __builtin_bswap32(SYSC_IDLEMODE_NOIDLE | SYSC_CLKACTIVITY_BOTH);

  // configure i2c bus speed
  i2c->psc = __builtin_bswap32(SYSTEM_CLOCK / INTERNAL_CLOCK) - 1;
  uint32_t divider = (INTERNAL_CLOCK / 2) / bus_speed;
  i2c->scll = __builtin_bswap32(divider - 7);
  i2c->sclh = __builtin_bswap32(divider - 5);

  //  enable i2c contoller
  i2c->con |= __builtin_bswap32(CON_I2C_EN);
}

/* -------------------------------------------------------------------------- */

int am335x_i2c_read(enum am335x_i2c_controllers ctrl, uint8_t chip_id,
                    uint8_t reg, uint8_t* data, uint16_t data_len) {
  volatile struct am335x_i2c_ctrl* i2c = i2c_ctrl[ctrl];

  // clear buffers and former pending status flags
  i2c->buf |= __builtin_bswap32(BUF_RXFIFO_CLR | BUF_TXFIFO_CLR);
  i2c->irqstatus = __builtin_bswap32(IRQSTATUS_RAW_NACK | IRQSTATUS_RAW_BF);

  // poll bus busy condition
  while ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_BB)) != 0)
    ;

  // set slave address (identification of the slave chip)
  i2c->sa = __builtin_bswap32(chip_id);

  // --- write command word if required
  // define number of byte to transfer
  i2c->cnt = __builtin_bswap32(1);

  // start transfer as master & transmitter
  i2c->con |= __builtin_bswap32(CON_MST | CON_TRX | CON_STT | CON_STP);

  // wait for sending data bytes
  while ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_XRDY)) == 0)
    ;
  i2c->data = __builtin_bswap32(reg);

  // wait until transfer complete and check if done correctly
  if (wait_for_status(i2c, IRQSTATUS_RAW_BF) == 0) {
    if ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_NACK)) != 0)
      return -1;
  }

  // --- read specified number of data bytes
  // indicate number of bytes to read
  i2c->cnt = __builtin_bswap32(data_len);

  // initiate read data transfer
  i2c->con = (i2c->con & ~__builtin_bswap32(CON_TRX)) |
             __builtin_bswap32(CON_MST | CON_STT | CON_STP);

  // wait until data received and read them
  while (data_len > 0) {
    while ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_RRDY)) == 0)
      ;
    *data++ = __builtin_bswap32(i2c->data);
    data_len--;
    i2c->irqstatus = __builtin_bswap32(IRQSTATUS_RAW_RRDY);
  }

  // acknowlegde all status information
  i2c->irqstatus = i2c->irqstatus_raw;

  return 0;
}

/* -------------------------------------------------------------------------- */

int am335x_i2c_write(enum am335x_i2c_controllers ctrl, uint8_t chip_id,
                     uint8_t reg, const uint8_t* data, uint16_t data_len) {
  volatile struct am335x_i2c_ctrl* i2c = i2c_ctrl[ctrl];

  // clear buffers and former pending status flags
  i2c->buf |= __builtin_bswap32(BUF_RXFIFO_CLR | BUF_TXFIFO_CLR);
  i2c->irqstatus = __builtin_bswap32(IRQSTATUS_RAW_NACK | IRQSTATUS_RAW_BF);

  // poll bus busy condition
  while ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_BB)) != 0)
    ;

  // set slave address (identification of the slave chip)
  i2c->sa = __builtin_bswap32(chip_id);

  // define number of byte to transfer
  i2c->cnt = __builtin_bswap32(1 + data_len);

  // start transfer as master & transmitter
  i2c->con |= __builtin_bswap32(CON_MST | CON_TRX | CON_STT | CON_STP);

  // --- write command word
  // wait for sending data bytes
  while ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_XRDY)) == 0)
    ;
  i2c->data = __builtin_bswap32(reg);

  // --- write specified number of data bytes
  // wait until data received and read them
  while (data_len > 0) {
    // wait for sending data bytes
    while ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_XRDY)) == 0)
      ;
    i2c->data = __builtin_bswap32(*data++);
    i2c->irqstatus = __builtin_bswap32(IRQSTATUS_RAW_XRDY);
    data_len--;
  }

  // wait until transfer complete and check if done correctly
  if (wait_for_status(i2c, IRQSTATUS_RAW_BF) == 0) {
    if ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_NACK)) != 0)
      return -1;
  }

  // acknowlegde all status information
  i2c->irqstatus = i2c->irqstatus_raw;

  return 0;
}

/* -------------------------------------------------------------------------- */
__attribute__((optimize(0))) bool am335x_i2c_probe(
    enum am335x_i2c_controllers ctrl, uint8_t chip_id) {
  volatile struct am335x_i2c_ctrl* i2c = i2c_ctrl[ctrl];
  bool found = false;

  // clear former pending status flags
  i2c->irqstatus = __builtin_bswap32(IRQSTATUS_RAW_NACK | IRQSTATUS_RAW_BF);

  //  poll until bus busy condition is false
  while ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_BB)) != 0)
    ;

  // set slave address (identification of the slave chip)
  i2c->sa = __builtin_bswap32(chip_id);

  // set minimum number of byte to transfer (required by i2c controller)
  i2c->cnt = __builtin_bswap32(1);

  // start transfer as master & receiver
  i2c->con = (i2c->con & ~__builtin_bswap32(CON_TRX)) |
             __builtin_bswap32(CON_MST | CON_STT | CON_STP);

  // wait until transfer complete and check chip presence
  // if (wait_for_status(i2c, IRQSTATUS_RAW_BF) == 0) {
  wait_for_status(i2c, IRQSTATUS_RAW_BF);
  found = (i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_NACK)) == 0;
  //}

  // if device exists, then read dummy data byte and clear rx fifo
  if (found) {
    while ((i2c->irqstatus_raw & __builtin_bswap32(IRQSTATUS_RAW_RRDY)) == 0)
      ;
    uint32_t data = __builtin_bswap32(i2c->data);
    (void)data;
  }

  // acknowlegde all status information
  i2c->irqstatus = i2c->irqstatus_raw;

  return found;
}
