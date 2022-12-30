/*
 * am335x_pru.h
 *
 *  Created on: May 13, 2021
 *      Author: renee.cousins
 */

#ifndef LIBBBB_INC_AM335X_PRU_H_
#define LIBBBB_INC_AM335X_PRU_H_

#include <stdint.h>

/****************************************************************************************************
* Register Definitions
****************************************************************************************************/
#define PRU_CTRL_CONTROL                           (0x0U)
#define PRU_CTRL_STATUS                            (0x4U)
#define PRU_CTRL_WAKEUP_EN                         (0x8U)
#define PRU_CTRL_CYCLE                             (0xcU)
#define PRU_CTRL_STALL                             (0x10U)
#define PRU_CTRL_CTBIR0                            (0x20U)
#define PRU_CTRL_CTBIR1                            (0x24U)
#define PRU_CTRL_CTPPR0                            (0x28U)
#define PRU_CTRL_CTPPR1                            (0x2cU)

/****************************************************************************************************
* Field Definition Macros
****************************************************************************************************/

#define PRU_CTRL_CONTROL_SOFT_RST_N_SHIFT          (0U)
#define PRU_CTRL_CONTROL_SOFT_RST_N_MASK           (0x00000001U)

#define PRU_CTRL_CONTROL_ENABLE_SHIFT              (1U)
#define PRU_CTRL_CONTROL_ENABLE_MASK               (0x00000002U)

#define PRU_CTRL_CONTROL_SLEEPING_SHIFT            (2U)
#define PRU_CTRL_CONTROL_SLEEPING_MASK             (0x00000004U)

#define PRU_CTRL_CONTROL_COUNTER_ENABLE_SHIFT      (3U)
#define PRU_CTRL_CONTROL_COUNTER_ENABLE_MASK       (0x00000008U)

#define PRU_CTRL_CONTROL_SINGLE_STEP_SHIFT         (8U)
#define PRU_CTRL_CONTROL_SINGLE_STEP_MASK          (0x00000100U)

#define PRU_CTRL_CONTROL_BIG_ENDIAN_SHIFT          (14U)
#define PRU_CTRL_CONTROL_BIG_ENDIAN_MASK           (0x00004000U)

#define PRU_CTRL_CONTROL_RUNSTATE_SHIFT            (15U)
#define PRU_CTRL_CONTROL_RUNSTATE_MASK             (0x00008000U)

#define PRU_CTRL_CONTROL_PCOUNTER_RST_VAL_SHIFT    (16U)
#define PRU_CTRL_CONTROL_PCOUNTER_RST_VAL_MASK     (0xffff0000U)

#define PRU_CTRL_STATUS_PCOUNTER_SHIFT             (0U)
#define PRU_CTRL_STATUS_PCOUNTER_MASK              (0x0000ffffU)

#define PRU_CTRL_WAKEUP_EN_BITWISE_ENABLES_SHIFT   (0U)
#define PRU_CTRL_WAKEUP_EN_BITWISE_ENABLES_MASK    (0xffffffffU)

#define PRU_CTRL_CYCLE_CYCLECOUNT_SHIFT            (0U)
#define PRU_CTRL_CYCLE_CYCLECOUNT_MASK             (0xffffffffU)

#define PRU_CTRL_STALL_STALLCOUNT_SHIFT            (0U)
#define PRU_CTRL_STALL_STALLCOUNT_MASK             (0xffffffffU)

#define PRU_CTRL_CTBIR0_C24_BLK_INDEX_SHIFT        (0U)
#define PRU_CTRL_CTBIR0_C24_BLK_INDEX_MASK         (0x000000ffU)

#define PRU_CTRL_CTBIR0_C25_BLK_INDEX_SHIFT        (16U)
#define PRU_CTRL_CTBIR0_C25_BLK_INDEX_MASK         (0x00ff0000U)

#define PRU_CTRL_CTBIR1_C26_BLK_INDEX_SHIFT        (0U)
#define PRU_CTRL_CTBIR1_C26_BLK_INDEX_MASK         (0x000000ffU)

#define PRU_CTRL_CTBIR1_C27_BLK_INDEX_SHIFT        (16U)
#define PRU_CTRL_CTBIR1_C27_BLK_INDEX_MASK         (0x00ff0000U)

#define PRU_CTRL_CTPPR0_C28_POINTER_SHIFT          (0U)
#define PRU_CTRL_CTPPR0_C28_POINTER_MASK           (0x0000ffffU)

#define PRU_CTRL_CTPPR0_C29_POINTER_SHIFT          (16U)
#define PRU_CTRL_CTPPR0_C29_POINTER_MASK           (0xffff0000U)

#define PRU_CTRL_CTPPR1_C30_POINTER_SHIFT          (0U)
#define PRU_CTRL_CTPPR1_C30_POINTER_MASK           (0x0000ffffU)

#define PRU_CTRL_CTPPR1_C31_POINTER_SHIFT          (16U)
#define PRU_CTRL_CTPPR1_C31_POINTER_MASK           (0xffff0000U)

#ifdef __cplusplus
extern "C" {
#endif

// AM335x Base Address for PRU-ICSS
#define SOC_PRUICSS1_REGS				(0x4A300000)

#define SOC_PRUICSS_PRU0_DRAM_OFFSET	(0x00000000)
#define SOC_PRUICSS_PRU1_DRAM_OFFSET	(0x00002000)
#define SOC_PRUICSS_SHARED_RAM_OFFSET	(0x00010000)
#define SOC_PRUICSS_INTC_OFFSET			(0x00020000)
#define SOC_PRUICSS_PRU0_CTRL_OFFSET	(0x00022000)
#define SOC_PRUICSS_PRU0_DBG_OFFSET		(0x00022400)
#define SOC_PRUICSS_PRU1_CTRL_OFFSET	(0x00024000)
#define SOC_PRUICSS_PRU1_DBG_OFFSET		(0x00024400)
#define SOC_PRUICSS_CFG_OFFSET			(0x00026000)
#define SOC_PRUICSS_UART_OFFSET			(0x00028000)
#define SOC_PRUICSS_IEP_OFFSET			(0x0002E000)
#define SOC_PRUICSS_ECAP_OFFSET			(0x00030000)
#define SOC_PRUICSS_PRU0_IRAM_OFFSET	(0x00034000)
#define SOC_PRUICSS_PRU1_IRAM_OFFSET	(0x00038000)

typedef enum {
    PRU0_DRAM,
    PRU1_DRAM,
    PRU0_IRAM,
    PRU1_IRAM,
    PRU_SHARED_RAM
} PRU_RAM_t;

typedef enum {
    NO_CORE,
    PRU0,
    PRU1
} PRU_CORE_t;

extern void am335x_pru_init(void);
extern void am335x_pru_reset(void);

extern void am335x_pru_halt(PRU_CORE_t PRUCore);
extern void am335x_pru_enable(PRU_CORE_t PRUCore);

extern void am335x_pru_memset(PRU_RAM_t MemoryType, uint32_t offset, uint32_t Length, const uint32_t *Pointer);
extern void am335x_pru_memcpy(PRU_RAM_t MemoryType, uint32_t offset, uint32_t Length, uint32_t Pattern);

#ifdef __cplusplus
}
#endif


#endif /* LIBBBB_INC_AM335X_PRU_H_ */
