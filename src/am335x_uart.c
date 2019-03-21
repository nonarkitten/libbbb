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
 * Project:	HEIA-FR / Embedded Systems 1+2 Laboratory
 *
 * Abstract: 	AM335x Universal Asynchronous Receiver/Transmitter (UART) Driver 
 *
 * Purpose:	This module implements basic services to drive the AM335x UART
 *		as serial user interface mainly used by the standard input/output 
 *		functions of the standard C library
 *
 * Author: 	Daniel Gachet
 * Date: 	03.07.2015
 */

#include "am335x_uart.h"
#include "am335x_clock.h"
#include "am335x_mux.h"

// define am335x uart controller registers
// be careful, most of those registers have multiple usages according to 
// a specific operation mode. Below defines 
struct am335x_uart_ctrl {
	uint32_t dll_rhr_thr; 	// 00
	uint32_t dlh_ier; 	// 04
	uint32_t iir_fcr_efr;	// 08
	uint32_t lcr;		// 0c
	uint32_t mcr_xon1_addr1;// 10
	uint32_t lsr_xon2_addr2;// 14
	uint32_t msr_tcr_xoff1;	// 18
	uint32_t tlr_spr_xoff2;	// 1c
	uint32_t mdr1;		// 20
	uint32_t mdr2;		// 24
	uint32_t r1[4]; 	// 28-34
	uint32_t uasr;		// 38
	uint32_t r2[1]; 	// 3c
	uint32_t scr;		// 40
	uint32_t ssr;		// 44
	uint32_t r3[2];		// 48-4c
	uint32_t mvr;		// 50
	uint32_t sysc;		// 54
	uint32_t syss;		// 58
	uint32_t wer;		// 5c
	uint32_t r4[1];		// 60
	uint32_t rxfifo_lvl;	// 64
	uint32_t txfifo_lvl;	// 68
	uint32_t ier2;		// 6c
	uint32_t isr2;		// 70
	uint32_t freq_sel;	// 74
	uint32_t r5[2];		// 78-7c
	uint32_t mdr3;		// 80
	uint32_t tx_dma_threshold;
};

#define dll		dll_rhr_thr
#define rhr		dll_rhr_thr
#define thr		dll_rhr_thr
#define dlh		dlh_ier
#define ier		dlh_ier
#define efr		iir_fcr_efr
#define fcr		iir_fcr_efr
#define iir		iir_fcr_efr
#define mcr		mcr_xon1_addr1
#define xon1_addr1	mcr_xon1_addr1
#define lsr		lsr_xon2_addr2
#define xon2_addr2	lsr_xon2_addr2
#define msr		msr_tcr_xoff1
#define tcr		msr_tcr_xoff1
#define	xoff1		msr_tcr_xoff1
#define spr		tlr_spr_xoff2
#define tlr		tlr_spr_xoff2
#define xoff2		tlr_spr_xoff2

/* UART EFR register bit definition */
#define EFR_AUTO_CTS_EN				(1<<7)
#define EFR_AUTO_RTS_EN				(1<<6)
#define EFR_SPECIAL_CHAR_DETECT			(1<<5)
#define EFR_ENHANCED_EN				(1<<4)

#define EFR_SW_FLOW_CONTROL_TX_MASK   		(0x3<<2)
#define EFR_SW_FLOW_CONTROL_TX_NONE   		(0x0<<2)
#define EFR_SW_FLOW_CONTROL_TX_XONOFF2		(0x1<<2)
#define EFR_SW_FLOW_CONTROL_TX_XONOFF1   	(0x2<<2)
#define EFR_SW_FLOW_CONTROL_TX_XONOFF1AND2	(0x3<<2)

#define EFR_SW_FLOW_CONTROL_RX_MASK		(0x3<<0)
#define EFR_SW_FLOW_CONTROL_RX_NONE		(0x0<<0)
#define EFR_SW_FLOW_CONTROL_RX_XONOFF2		(0x1<<0)
#define EFR_SW_FLOW_CONTROL_RX_XONOFF1		(0x2<<0)
#define EFR_SW_FLOW_CONTROL_RX_XONOFF1AND2 	(0x3<<0)

/* UART FCR register bit definition */
#define FCR_RX_FIFO_TRIG			(0x3<<6)
#define FCR_RX_FIFO_TRIG_8CHAR			(0x0<<6)
#define FCR_RX_FIFO_TRIG_16CHAR			(0x1<<6)
#define FCR_RX_FIFO_TRIG_56CHAR			(0x2<<6)
#define FCR_RX_FIFO_TRIG_60CHAR			(0x3<<6)

#define FCR_TX_FIFO_TRIG_MASK			(0x3<<4)
#define FCR_TX_FIFO_TRIG_8SPACES		(0x0<<4)
#define FCR_TX_FIFO_TRIG_16SPACES		(0x1<<4)
#define FCR_TX_FIFO_TRIG_32SPACES		(0x2<<4)
#define FCR_TX_FIFO_TRIG_56SPACES		(0x3<<4)

#define FCR_DMA_MODE				(1<<3)
#define FCR_TX_FIFO_CLEAR			(1<<2)
#define FCR_RX_FIFO_CLEAR			(1<<1)
#define FCR_FIFO_EN				(1<<0)

/* UART LCR register bit definition */
#define LCR_DIV_EN				(1<<7)
#define LCR_BREAK_EN				(1<<6)
#define LCR_PARITY_TYPE2			(1<<5)
#define LCR_PARITY_TYPE1			(1<<4)
#define LCR_PARITY_EN				(1<<3)
#define LCR_NB_STOP				(1<<2)

#define LCR_CHAR_LENGTH_MASK			(0x3<<0)
#define LCR_CHAR_LENGTH_5BIT			(0x0<<0)
#define LCR_CHAR_LENGTH_6BIT			(0x1<<0)
#define LCR_CHAR_LENGTH_7BIT			(0x2<<0)
#define LCR_CHAR_LENGTH_8BIT			(0x3<<0)

#define LCR_OPMODE_A				(0x80)
#define LCR_OPMODE_B				(0xbf)
#define LCR_OPMODE_NORMAL			(0x00)

/* UART SCR register bit definition */
#define SCR_RX_TRIG_GRANU1			(1<<7)
#define SCR_TX_TRIG_GRANU1			(1<<6)
#define SCR_DSR_IT				(1<<5)
#define SCR_RX_CTS_DSR_WAKE_UP_ENABLE		(1<<4)
#define SCR_TX_EMPTY_CTL_IT			(1<<3)

#define SCR_DMA_MODE_2				(0x3<<1)
#define SCR_DMA_MODE_2_MODE0			(0x0<<1)
#define SCR_DMA_MODE_2_MODE1			(0x1<<1)
#define SCR_DMA_MODE_2_MODE2			(0x2<<1)
#define SCR_DMA_MODE_2_MODE3			(0x3<<1)

#define SCR_DMA_MODE_CTL  			(1<<0)

/* UART MCR register bit definition */
#define MCR_TCR_TLR				(1<<6)
#define MCR_XON_EN				(1<<5)
#define MCR_LOOPBACK_EN				(1<<4)
#define MCR_CD_STS_CH				(1<<3)
#define MCR_RI_STS_CH				(1<<2)
#define MCR_RTS					(1<<1)
#define MCR_DTR					(1<<0)

/* UART MDR1 register bit definition */
#define MDR1_FRAME_END_MODE			(1<<7)
#define MDR1_SIP_MODE				(1<<6)
#define MDR1_SCT				(1<<5)		
#define MDR1_SET_TXIR				(1<<4)
#define MDR1_IR_SLEEP				(1<<3)

#define MDR1_MODE_SELECT   			(0x7<<0)
#define MDR1_MODE_SELECT_UART16X		(0x0<<0)
#define MDR1_MODE_SELECT_SIR			(0x1<<0)
#define MDR1_MODE_SELECT_UART16XAUTO		(0x2<<0)
#define MDR1_MODE_SELECT_UART13X		(0x3<<0)
#define MDR1_MODE_SELECT_MIR			(0x4<<0)
#define MDR1_MODE_SELECT_FIR			(0x5<<0)
#define MDR1_MODE_SELECT_CIR			(0x6<<0)
#define MDR1_MODE_SELECT_DISABLED		(0x7<<0)

/* UART SYSC register bit definition */
#define SYSC_IDLEMODE_MASK   			(3<<3)
#define SYSC_IDLEMODE_FORCE   			(0x0<<3)
#define SYSC_IDLEMODE_NOIDLE			(0x1<<3)
#define SYSC_IDLEMODE_SMART			(0x2<<3)
#define SYSC_IDLEMODE_WAKEUP			(0x3<<3)
#define SYSC_ENAWAKEUP   			(1<<2)
#define SYSC_SOFTRESET				(1<<1)
#define SYSC_AUTOIDLE   			(1<<0)

/* UART SYSS register bit definition */
#define SYSS_RESETDONE   			(1<<0)

/* UART SSR register bit definition */
#define SSR_DMA_COUNTER_RST			(1<<2)
#define SSR_RX_CTS_DSR_WAKE_UP_STS		(1<<1)
#define SSR_TX_FIFO_FULL			(1<<0)

/* UART LSR register bit definition */
#define LSR_RX_FIFO_STS				(1<<7)
#define LSR_TX_SR_E				(1<<6)
#define LSR_TX_FIFO_E				(1<<5)
#define LSR_RX_BI				(1<<4)
#define LSR_RX_FE				(1<<3)
#define LSR_RX_PE				(1<<2)
#define LSR_RX_OE				(1<<1)
#define LSR_RX_FIFO_E				(1<<0)



// am335x uart controllers memory mapped access register pointers
static volatile struct am335x_uart_ctrl* uart_ctrl[] = {
	(struct am335x_uart_ctrl*)0x44e09000, // 0
	(struct am335x_uart_ctrl*)0x48022000, // 1
	(struct am335x_uart_ctrl*)0x48024000, // 2
	(struct am335x_uart_ctrl*)0x481a6000, // 3
	(struct am335x_uart_ctrl*)0x481a8000, // 4
	(struct am335x_uart_ctrl*)0x481aa000, // 5
};

// default uart configuration for serial line
#define DEFAULT_BAUDRATE	115200
#define UART_MODULE_INPUT_CLOCK	48000000


// table to convert uart interface to clock module number
static const enum am335x_clock_uart_modules uart2clock[] = {
	AM335X_CLOCK_UART0,
	AM335X_CLOCK_UART1,
	AM335X_CLOCK_UART2,
	AM335X_CLOCK_UART3,
	AM335X_CLOCK_UART4,
	AM335X_CLOCK_UART5,
};

// table to convert uart interface to clock module number
static const enum am335x_clock_uart_modules uart2mux[] = {
	AM335X_MUX_UART0,
	AM335X_MUX_UART1,
	AM335X_MUX_UART2,
	AM335X_MUX_UART3,
	AM335X_MUX_UART4,
	AM335X_MUX_UART5,
};


/* --------------------------------------------------------------------------
 * implementation of the public methods
 * -------------------------------------------------------------------------- */

void am335x_uart_init (enum am335x_uart_controllers ctrl)
{
	volatile struct am335x_uart_ctrl* uart = uart_ctrl[ctrl];

	// configure uart system clocks 
	am335x_clock_enable_uart_module(uart2clock[ctrl]);

	// setup uart pin multiplexing
	am335x_mux_setup_uart_pins(uart2mux[ctrl]);

	// reset uart controller and wait until reset complete
	uart->sysc |= SYSC_SOFTRESET;
	while ((uart->syss & SYSS_RESETDONE) == 0);

	// perform fifo configuration
	uart->lcr = LCR_OPMODE_B;	// switch to configuration mode B
	uint32_t efr = uart->efr;	// save efr register
	uart->efr = EFR_ENHANCED_EN;	// enable writing to IER, FCR & MCR regs
	uint32_t lcr = uart->lcr;	// save lcr register
	uart->lcr = LCR_OPMODE_A;	// switch to configuration mode A
	uint32_t mcr = uart->mcr;	// save mcr register
	uart->mcr = MCR_TCR_TLR;	// enable access to TCR & TLR regs
	uart->fcr = FCR_RX_FIFO_TRIG_60CHAR | FCR_TX_FIFO_TRIG_56SPACES
		  | FCR_TX_FIFO_CLEAR | FCR_RX_FIFO_CLEAR | FCR_FIFO_EN;

	uart->lcr = LCR_OPMODE_B;	// switch to configuration mode B
	uart->tlr = (60<<4) + (56<<0);	// set new fifo trigger level
	uart->scr = 0;			// disable all...
	uart->efr = efr;		// restore efr register
	uart->lcr = LCR_OPMODE_A;	// switch to configuration mode A
	uart->mcr = mcr;		// restore mcr register
	uart->lcr = lcr;		// restore lcr register

	// disable all interrupts and program line characteristics
	uart->mdr1 = MDR1_MODE_SELECT_DISABLED;	// disable uart 
	uart->lcr = LCR_OPMODE_B;	// switch to configuration mode B
	efr = uart->efr;		// save efr register
	uart->efr = EFR_ENHANCED_EN;	// enable writing to IER, FCR & MCR regs
	uart->lcr = LCR_OPMODE_NORMAL;	// switch mode access IER register
	uart->ier = 0x0;		// clear IER bits
	uart->lcr = LCR_OPMODE_B;
	uart->efr = efr;		// restore efr register
	uart->lcr = LCR_CHAR_LENGTH_8BIT; // 8 bit char
	
	// perform baudrate configuration
	am335x_uart_set_baudrate (ctrl, DEFAULT_BAUDRATE);
}

/* -------------------------------------------------------------------------- */

void am335x_uart_set_baudrate (
	enum am335x_uart_controllers ctrl, 
	uint32_t baudrate)
{
	volatile struct am335x_uart_ctrl* uart = uart_ctrl[ctrl];

	// disable uart
	uart->mdr1 = MDR1_MODE_SELECT_DISABLED; 

	// configure divider value 
	uint32_t lcr = uart->lcr;
	uart->lcr = LCR_OPMODE_B;
	if (baudrate > 230400) baudrate = 230400;
	uint32_t divider = (UART_MODULE_INPUT_CLOCK / 16) / baudrate;
	uart->dlh = divider / 256; 
	uart->dll = divider % 256;
	uart->lcr = lcr;

	// select uart 16x mode
	uart->mdr1 = MDR1_MODE_SELECT_UART16X;
}

/* -------------------------------------------------------------------------- */

bool am335x_uart_tstc(enum am335x_uart_controllers ctrl)
{
	volatile struct am335x_uart_ctrl* uart = uart_ctrl[ctrl];
	return ((uart->lsr & LSR_RX_FIFO_E) != 0);
}

/* -------------------------------------------------------------------------- */

int am335x_uart_read(enum am335x_uart_controllers ctrl)
{
	volatile struct am335x_uart_ctrl* uart = uart_ctrl[ctrl];
	while ((uart->lsr & LSR_RX_FIFO_E) == 0);
	return uart->rhr;
}

/* -------------------------------------------------------------------------- */

void am335x_uart_write(
	enum am335x_uart_controllers ctrl,
	int c)
{
	volatile struct am335x_uart_ctrl* uart = uart_ctrl[ctrl];
	while ((uart->ssr & SSR_TX_FIFO_FULL) != 0);
	uart->thr = c;
}


