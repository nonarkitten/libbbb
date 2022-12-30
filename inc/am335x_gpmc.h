#ifndef AM335X_GPMC_H
#define AM335X_GPMC_H

/*
 * am335x_gpmc.h
 *
 *  Created on: Apr. 29, 2021
 *      Author: renee.cousins
 */

#define GPMC_SIZE_256M	0x0
#define GPMC_SIZE_128M	0x8
#define GPMC_SIZE_64M	0xC
#define GPMC_SIZE_32M	0xE
#define GPMC_SIZE_16M	0xF

#include <stdbool.h>
#include <stdint.h>

#include "am335x_clock.h"

typedef enum {
	_0_DNU,
	AM335X_GPMC_CS1,
	AM335X_GPMC_CS2,
	AM335X_GPMC_CS3,
	AM335X_GPMC_CS4,
	AM335X_GPMC_CS5,
	AM335X_GPMC_CS6
} AM335X_GPMC_CS_t;

/* Structure to save gpmc cs context */
typedef struct {
	uint32_t config1; // 60
	uint32_t config2; // 64
	uint32_t config3; // 68
	uint32_t config4; // 6c

	uint32_t config5; // 70
	uint32_t config6; // 74
	uint32_t config7; // 78
	uint32_t nand_cmd;// 7c

	uint32_t nand_address; // 80
	uint32_t nand_data; // 84
	uint32_t __reserved_08;// 88
	uint32_t __reserved_0C;// 8c
	// c0
} am335x_gpmc_cs_config_t;

/*
 * Structure to save/restore gpmc context
 * to support core off on OMAP3
 */
typedef struct {
	uint32_t revision;
							uint32_t __reserved_04;
							uint32_t __reserved_08;
							uint32_t __reserved_0C;
	uint32_t sysconfig;
	uint32_t sysstatus;
	uint32_t irqstatus;
	uint32_t irqenable;
							uint32_t __reserved_20;
							uint32_t __reserved_24;
							uint32_t __reserved_28;
							uint32_t __reserved_2C;
							uint32_t __reserved_30;
							uint32_t __reserved_34;
							uint32_t __reserved_38;
							uint32_t __reserved_3C;
	uint32_t timeout_ctrl;
	uint32_t err_address;
	uint32_t err_type;
							uint32_t __reserved_4C;
	uint32_t config;
	uint32_t status;
							uint32_t __reserved_58;
							uint32_t __reserved_5C;

	am335x_gpmc_cs_config_t cs_context[6];

	uint32_t prefetch_config1;
	uint32_t prefetch_config2;
	uint32_t prefetch_control;
	uint32_t prefetch_status;
	// ecc bla bla bla
} am335x_gpmc_regs_t;

extern void am335x_gpmc_enable_cs_config(
		const am335x_gpmc_cs_config_t* setting,	// configuration settings
		uint32_t cs,						// chip select to use (1-6)
		uint32_t base, 						// address where GPMC ought to be
		uint32_t size						// size of this range
		);

extern void am335x_gpmc_init(uint32_t clk_div);

#endif /* LIBBBB_INC_AM335X_GPMC_H_ */
