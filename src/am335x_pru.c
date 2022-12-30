/*
 * am335x_pru.c
 *
 *  Created on: May 13, 2021
 *      Author: renee.cousins
 */

#include "am335x_pru.h"

#include <string.h>

#include "am335x_clock.h"

static volatile struct am335x_prm_irq {
  uint32_t revision_prm;
  uint32_t prm_irqstatus_mpu;
  uint32_t prm_irqenable_mpu;
  uint32_t prm_irqstatus_m3;
  uint32_t prm_irqenable_m3;
} *prm_irq = (volatile struct am335x_prm_irq *)0x44E00B00;

static volatile struct am335x_prm_per {
  uint32_t rm_per_rstctrl;
  uint32_t res1[1];
  uint32_t pm_per_pwrstst;
  uint32_t pm_per_pwrstctrl;
} *prm_per = (volatile struct am335x_prm_per *)0x44E00C00;

static volatile struct am335x_prm_wkup {
  uint32_t rm_wkup_rstctrl;
  uint32_t pm_wkup_pwrstctrl;
  uint32_t pm_wkup_pwrstst;
  uint32_t rm_wkup_rsts;
} *prm_wkup = (volatile struct am335x_prm_wkup *)0x44E00D00;

static volatile struct am335x_prm_mpu {
  uint32_t pm_mpu_pwrstctrl;
  uint32_t pm_mpu_pwrstst;
  uint32_t rm_mpu_rstst;
} *prm_mpu = (volatile struct am335x_prm_mpu *)0x44E00E00;

static volatile struct am335x_prm_device {
  uint32_t rm_wkup_rstctrl;
  uint32_t pm_wkup_pwrstctrl;
  uint32_t pm_wkup_pwrstst;
  uint32_t rm_wkup_rsts;
} *prm_device = (volatile struct am335x_prm_device *)0x44E00F00;

static volatile struct am335x_pruss_cfg {
  uint32_t revid;
  uint32_t syscfg;
  uint32_t gpcfg0;
  uint32_t gpcfg1;
  uint32_t cgr;
  uint32_t isrp;
  uint32_t isp;
  uint32_t iesp;
  uint32_t iecp;
  uint32_t scrp;
  uint32_t pmao;
  uint32_t mii_rt;
  uint32_t iepclk;
  uint32_t spp;
  uint32_t rev1[1];
  uint32_t pin_mx;
} *pruss_cfg = (volatile struct am335x_pruss_cfg *)0x4A326000;

static volatile struct am335x_pru_ctrl {
  uint32_t ctrl;
  uint32_t status;
  uint32_t wakeup_en;
  uint32_t cycle;
  uint32_t stall;
  uint32_t res1[3];  // 14, 18, 1c
  uint32_t ctbir0;
  uint32_t ctbir1;
  uint32_t ctppr0;
  uint32_t ctppr1;
} *pru0_ctrl = (volatile struct am335x_pru_ctrl*)0x4A322000,
  *pru1_ctrl = (volatile struct am335x_pru_ctrl*)0x4A324000;

void am335x_pru_init(void) {
  /* Enable PRU Clocks */
  am335x_clock_enable_pru(&prm_per->rm_per_rstctrl);

  am335x_dmtimer1_wait_us(100);

  pruss_cfg->syscfg = __builtin_bswap32(0x00000005);
  while (pruss_cfg->syscfg & __builtin_bswap32(0x20))
    ;

  /* Clear out the date memory of both PRU cores */
  am335x_pru_memset(PRU0_DRAM, 0, 8 * 1024, 0);  // Data 8KB RAM0
  am335x_pru_memset(PRU1_DRAM, 0, 8 * 1024, 0);  // Data 8KB RAM1
  am335x_pru_memset(PRU0_IRAM, 0, 8 * 1024, 0);
  am335x_pru_memset(PRU1_IRAM, 0, 8 * 1024, 0);
}

void am335x_pru_halt(PRU_CORE_t PRUCore) {
  if (PRUCore == PRU0) pru0_ctrl->ctrl = 0;
  if (PRUCore == PRU1) pru1_ctrl->ctrl = 0;
}

void am335x_pru_enable(PRU_CORE_t PRUCore) {
  if (PRUCore & PRU0) pru0_ctrl->ctrl = __builtin_bswap32(0xB);
  if (PRUCore & PRU1) pru1_ctrl->ctrl = __builtin_bswap32(0xB);
}

void am335x_pru_reset(void) {}

static uint32_t am335x_pru_mem_base(PRU_RAM_t MemoryType) {
  if (MemoryType == PRU0_DRAM)
    return SOC_PRUICSS1_REGS + SOC_PRUICSS_PRU0_DRAM_OFFSET;
  else if (MemoryType == PRU1_DRAM)
    return SOC_PRUICSS1_REGS + SOC_PRUICSS_PRU1_DRAM_OFFSET;
  else if (MemoryType == PRU0_IRAM)
    return SOC_PRUICSS1_REGS + SOC_PRUICSS_PRU0_IRAM_OFFSET;
  else if (MemoryType == PRU1_IRAM)
    return SOC_PRUICSS1_REGS + SOC_PRUICSS_PRU1_IRAM_OFFSET;
  else if (MemoryType == PRU_SHARED_RAM)
    return SOC_PRUICSS1_REGS + SOC_PRUICSS_SHARED_RAM_OFFSET;
  else
    return 0;
}

void am335x_pru_memcpy(PRU_RAM_t MemoryType, uint32_t offset, uint32_t Length,
                       const uint32_t *Pointer) {
  uint8_t *srcaddr = (uint8_t *)Pointer;
  uint8_t *destaddr = (uint8_t *)(am335x_pru_mem_base(MemoryType) + offset);

  memcpy(destaddr, srcaddr, Length);
}

void am335x_pru_memset(PRU_RAM_t MemoryType, uint32_t offset, uint32_t Length,
                       uint32_t Pattern) {
  uint8_t *destaddr = (uint8_t *)(am335x_pru_mem_base(MemoryType) + offset);
  memset(destaddr, Pattern, (Length / 4));
}