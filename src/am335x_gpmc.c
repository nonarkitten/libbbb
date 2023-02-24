/*
 * am335x_gpmc.c
 *
 *  Created on: Apr. 29, 2021
 *      Author: renee.cousins
 */

#include <stdint.h>

#include "support.h"
#include "am335x_gpmc.h"

static volatile const am335x_gpmc_regs_t* gpmc = (am335x_gpmc_regs_t*)0x50000000;

static void writel(uint32_t value, uint32_t* address) {
	*(volatile uint32_t*)address = LE32(value);
	asm volatile("" : : : "memory");
}

void am335x_gpmc_enable_cs_config(
		uint32_t* config,
		uint32_t cs,
		uint32_t base,
		uint32_t size)
{
	// Point to the GPMC config
	am335x_gpmc_cs_config_t* _cs = (am335x_gpmc_cs_config_t*)&gpmc->cs_context[cs];
	am335x_gpmc_cs_config_t* setting = (am335x_gpmc_cs_config_t*)config;

	// Take an actual size and convert it to an address mask
	size = (size > 0x08000000) ? GPMC_SIZE_256M :
	      ((size > 0x04000000) ? GPMC_SIZE_128M :
	      ((size > 0x02000000) ? GPMC_SIZE_64M  :
	      ((size > 0x01000000) ? GPMC_SIZE_32M  :
	                             GPMC_SIZE_16M)));

	// Ensure base aligns to the boundary
	base &= ((0xF0 | size) << 24);

	writel(0, &_cs->config7);
	am335x_dmtimer1_wait_us(1000);

	/* Delay for settling */
	writel(setting->config1, &_cs->config1);
	writel(setting->config2, &_cs->config2);
	writel(setting->config3, &_cs->config3);
	writel(setting->config4, &_cs->config4);
	writel(setting->config5, &_cs->config5);
	writel(setting->config6, &_cs->config6);

	/* Enable the config */
	writel((((size & 0xF) << 8) |
			((base >> 24) & 0x3F) |
			(1 << 6)),
			&_cs->config7);

	am335x_dmtimer1_wait_us(2000);
}

void am335x_gpmc_init(uint32_t clk_div)
{
	am335x_clock_enable_gpmc(clk_div);

	/* Reset GPMC */
	writel(2, (uint32_t*)&gpmc->sysconfig);
	while(!gpmc->sysstatus) continue;

	/* global settings */
	writel(0, (uint32_t*)&gpmc->irqenable); /* isr's sources masked */
	writel(0, (uint32_t*)&gpmc->timeout_ctrl);/* timeout disable */
	writel(1 << 8, (uint32_t*)&gpmc->config); /* wait polarity */

	/*
	 * Disable the GPMC0 config set by ROM code
	 * It conflicts with our MPDB (both at 0x08000000)
	 */
	writel(0, (uint32_t*)&gpmc->cs_context[0].config7);
}
