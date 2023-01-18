/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ */
/*
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 * 
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 * 
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#pragma once

#ifndef __AM335x_IRQ_H__
#define __AM335x_IRQ_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
**                       MACRO DEFINITIONS
****************************************************************************/
/*
** Macros which can be passed to IntPrioritySet API as hostIntRoute.
*/
/* To route an interrupt to IRQ */
#define AINTC_HOSTINT_ROUTE_IRQ    (0)
/* To route an interrupt to FIQ */
#define AINTC_HOSTINT_ROUTE_FIQ    (INTC_ILR_FIQNIRQ)
/*
** Interrupt number list
*/
#define SYS_INT_EMUINT             (0)
#define SYS_INT_COMMTX             (1)
#define SYS_INT_COMMRX             (2)
#define SYS_INT_BENCH              (3)
#define SYS_INT_ELM_IRQ            (4)
#define SYS_INT_SSM_WFI_IRQ        (5)
#define SYS_INT_SSM_IRQ            (6)
#define SYS_INT_NMI                (7)
#define SYS_INT_SEC_EVNT           (8)
#define SYS_INT_L3DEBUG            (9)
#define SYS_INT_L3APPINT           (10)
#define SYS_INT_PRCMINT            (11)
#define SYS_INT_EDMACOMPINT        (12)
#define SYS_INT_EDMAMPERR          (13)
#define SYS_INT_EDMAERRINT         (14)
#define SYS_INT_WDT0INT            (15)
#define SYS_INT_ADC_TSC_GENINT     (16)
#define SYS_INT_USBSSINT           (17)
#define SYS_INT_USB0               (18)
#define SYS_INT_USB1               (19)
#define SYS_INT_PRUSS1_EVTOUT0     (20)
#define SYS_INT_PRUSS1_EVTOUT1     (21)
#define SYS_INT_PRUSS1_EVTOUT2     (22)
#define SYS_INT_PRUSS1_EVTOUT3     (23)
#define SYS_INT_PRUSS1_EVTOUT4     (24)
#define SYS_INT_PRUSS1_EVTOUT5     (25)
#define SYS_INT_PRUSS1_EVTOUT6     (26)
#define SYS_INT_PRUSS1_EVTOUT7     (27)
#define SYS_INT_MMCSD1INT          (28)
#define SYS_INT_MMCSD2INT          (29)
#define SYS_INT_I2C2INT            (30)
#define SYS_INT_eCAP0INT           (31)
#define SYS_INT_GPIOINT2A          (32)
#define SYS_INT_GPIOINT2B          (33)
#define SYS_INT_USBWAKEUP          (34)
#define SYS_INT_LCDCINT            (36)
#define SYS_INT_GFXINT             (37)
#define SYS_INT_2DHWAINT           (38)
#define SYS_INT_ePWM2INT           (39)
#define SYS_INT_3PGSWRXTHR0        (40)
#define SYS_INT_3PGSWRXINT0        (41)
#define SYS_INT_3PGSWTXINT0        (42)
#define SYS_INT_3PGSWMISC0         (43)
#define SYS_INT_UART3INT           (44)
#define SYS_INT_UART4INT           (45)
#define SYS_INT_UART5INT           (46)
#define SYS_INT_eCAP1INT           (47)
#define SYS_INT_DCAN0_INT0         (52)
#define SYS_INT_DCAN0_INT1         (53)
#define SYS_INT_DCAN0_PARITY       (54)
#define SYS_INT_DCAN1_INT0         (55)
#define SYS_INT_DCAN1_INT1         (56)
#define SYS_INT_DCAN1_PARITY       (57)
#define SYS_INT_ePWM0_TZINT        (58)
#define SYS_INT_ePWM1_TZINT        (59)
#define SYS_INT_ePWM2_TZINT        (60)
#define SYS_INT_eCAP2INT           (61)
#define SYS_INT_GPIOINT3A          (62)
#define SYS_INT_GPIOINT3B          (63)
#define SYS_INT_MMCSD0INT          (64)
#define SYS_INT_SPI0INT            (65)
#define SYS_INT_TINT0              (66)
#define SYS_INT_TINT1_1MS          (67)
#define SYS_INT_TINT2              (68)
#define SYS_INT_TINT3              (69)
#define SYS_INT_I2C0INT            (70)
#define SYS_INT_I2C1INT            (71)
#define SYS_INT_UART0INT           (72)
#define SYS_INT_UART1INT           (73)
#define SYS_INT_UART2INT           (74)
#define SYS_INT_RTCINT             (75)
#define SYS_INT_RTCALARMINT        (76)
#define SYS_INT_MBINT0             (77)
#define SYS_INT_M3_TXEV            (78)
#define SYS_INT_eQEP0INT           (79)
#define SYS_INT_MCATXINT0          (80)
#define SYS_INT_MCARXINT0          (81)
#define SYS_INT_MCATXINT1          (82)
#define SYS_INT_MCARXINT1          (83)
#define SYS_INT_ePWM0INT           (86)
#define SYS_INT_ePWM1INT           (87)
#define SYS_INT_eQEP1INT           (88)
#define SYS_INT_eQEP2INT           (89)
#define SYS_INT_DMA_INTR_PIN2      (90)
#define SYS_INT_WDT1INT            (91)
#define SYS_INT_TINT4              (92)
#define SYS_INT_TINT5              (93)
#define SYS_INT_TINT6              (94)
#define SYS_INT_TINT7              (95)
#define SYS_INT_GPIOINT0A          (96)
#define SYS_INT_GPIOINT0B          (97)
#define SYS_INT_GPIOINT1A          (98)
#define SYS_INT_GPIOINT1B          (99)
#define SYS_INT_GPMCINT            (100)
#define SYS_INT_DDRERR0            (101)
#define SYS_INT_AES0_IRQ_S         (102)
#define SYS_INT_AES0_IRQ_P         (103)
#define SYS_INT_SHA_IRQ_S          (108)
#define SYS_INT_SHA_IRQ_P          (109)
#define SYS_INT_FPKA_SINTREQUEST_S (110)
#define SYS_INT_RNG_IRQ            (111)
#define SYS_INT_TCERRINT0          (112)
#define SYS_INT_TCERRINT1          (113)
#define SYS_INT_TCERRINT2          (114)
#define SYS_INT_SMRFLX_Sabertooth  (120)
#define SYS_INT_SMRFLX_Core        (121)
#define SYS_INT_DMA_INTR_PIN0      (123)
#define SYS_INT_DMA_INTR_PIN1      (124)
#define SYS_INT_SPI1INT            (125)
/*****************************************************************************
**                     API FUNCTION PROTOTYPES
*****************************************************************************/
/* \brief   This API is used to initialize the interrupt controller. This API
 *          shall be called before using the interrupt controller. */
extern void IntAINTCInit(void);

/* \brief   Sets the interface clock to be free running
 * \param   None. */
extern void IntIfClkFreeRunSet(void);

/* \brief   When this API is called,  automatic clock gating strategy is applied
 *          based on the interface bus activity.
 * \param   None. */
extern void IntIfClkAutoGateSet(void);

/* \brief   Enables protection mode for the interrupt controller register
 *          access. When the protection is enabled, the registers will be accessible only
 *          in privileged mode of the CPU. */
extern void IntProtectionEnable(void);

/* \brief   Disables protection mode for the interrupt controller register
 *          access. When the protection is disabled, the registers will be accessible in
 *          both unprivileged and privileged mode of the CPU. */
extern void IntProtectionDisable(void);

/* \brief   Enables the free running of input synchronizer clock */
extern void IntSyncClkFreeRunSet(void);

/* \brief   When this API is called, Input synchronizer clock is auto-gated
 *          based on interrupt input activity */
extern void IntSyncClkAutoGateSet(void);

/* \brief   Enables the free running of functional clock */
extern void IntFuncClkFreeRunSet(void);

/* \brief   When this API is called, functional clock gating strategy
 *          is applied. */
extern void IntFuncClkAutoGateSet(void);

/* \brief   Sets the given priority threshold value.
 * \param   threshold - Priority threshold value
 *      'threshold' can take any value from 0x00 to 0x7F. To disable
 *      priority threshold, write 0xFF. */
extern void IntPriorityThresholdSet(uint32_t threshold);

/* \brief   Sets software interrupt for the given interrupt number.
 * \param   intrNum - the system interrupt number, for which software interrupt
 *                    to be generated */
extern void IntSoftwareIntSet(uint32_t intrNum);

/* \brief   Clears the software interrupt for the given interrupt number.
 * \param   intrNum - the system interrupt number, for which software interrupt
 *                    to be cleared. */
extern void IntSoftwareIntClear(uint32_t intrNum);

/* \brief  Enables the processor IRQ only in CPSR. Makes the processor to
 *         respond to IRQs.  This does not affect the set of interrupts
 *         enabled/disabled in the AINTC.
 * \note  This function call shall be done only in previleged mode of ARM */
extern void IntMasterIRQEnable(void);

/* \brief  Disables the processor IRQ only in CPSR.Prevents the processor to
 *         respond to IRQs.  This does not affect the set of interrupts
 *         enabled/disabled in the AINTC.
 * \note  This function call shall be done only in previleged mode of ARM */
extern void IntMasterIRQDisable(void);

/* \brief  Enables the processor FIQ only in CPSR. Makes the processor to
 *         respond to FIQs.  This does not affect the set of interrupts
 *         enabled/disabled in the AINTC.
 * \note  This function call shall be done only in previleged mode of ARM */
extern void IntMasterFIQEnable(void);

/* \brief  Disables the processor FIQ only in CPSR.Prevents the processor to
 *         respond to FIQs.  This does not affect the set of interrupts
 *         enabled/disabled in the AINTC.
 * \note  This function call shall be done only in previleged mode of ARM */
extern void IntMasterFIQDisable(void);

/* \brief   This API enables the system interrupt in AINTC. However, for
 *          the interrupt generation, make sure that the interrupt is
 *          enabled at the peripheral level also.
 * \param   intrNum  - Interrupt number */
extern void IntSystemEnable(uint32_t intrNum);

/* \brief   This API disables the system interrupt in AINTC.
 * \param   intrNum  - Interrupt number */
extern void IntSystemDisable(uint32_t intrNum);

/* \brief   Unregisters an interrupt
 * \param   intrNum - Interrupt Number
 * \note	Once an interrupt is unregistered it will enter infinite loop once
 * an interrupt occurs */
extern void IntUnRegister(uint32_t intrNum);

/* \brief  Restore the processor IRQ only status. This does not affect
 *          the set of interrupts enabled/disabled in the AINTC.
 * \param    The status returned by the IntDisable fundtion.
 * \note  This function call shall be done only in previleged mode of ARM */
extern void IntEnable(uint8_t status);

/* \brief    Registers an interrupt Handler in the interrupt vector table for
 *           system interrupts.
 * \param    intrNum - Interrupt Number
 * \param    fnHandler - Function pointer to the ISR
 * \note	When the interrupt occurs for the sytem interrupt number indicated,
 * the control goes to the ISR given as the parameter. */
extern void IntRegister(uint32_t intrNum, void (*pfnHandler)(void));

/* \brief   This API assigns a priority to an interrupt and routes it to
 *          either IRQ or to FIQ. Priority 0 is the highest priority level
 *          Among the host interrupts, FIQ has more priority than IRQ.
 * \param   intrNum  - Interrupt number
 * \param   priority - Interrupt priority level from 0 (low) to 127 (high)
 * \param   hostIntRoute - The host interrupt handler:
 *          AINTC_HOSTINT_ROUTE_IRQ, AINTC_HOSTINT_ROUTE_FIQ */
extern void IntPrioritySet(uint32_t intrNum, uint32_t priority, uint32_t hostIntRoute);

/* \brief  Read and save the stasus and Disables the processor IRQ .
 *         Prevents the processor to respond to IRQs.
 * \return   Current status of IRQ
 * \note  This function call shall be done only in previleged mode of ARM */
extern uint8_t IntDisable(void);

/* \brief   Returns the status of the interrupts FIQ and IRQ.
 * \return   Status of interrupt as in CPSR.
 * \note  This function call shall be done only in previleged mode of ARM */
extern uint32_t IntMasterStatusGet(void);

/* \brief   Reads the active IRQ number.
 * \return  Active IRQ number. */
extern uint32_t IntActiveIrqNumGet(void);

/* \brief   Reads the active FIQ number.
 * \return  Active FIQ number. */
extern uint32_t IntActiveFiqNumGet(void);

/* \brief   Reads the spurious IRQ Flag. Spurious IRQ flag is reflected in both
 *          SIR_IRQ and IRQ_PRIORITY registers of the interrupt controller.
 * \return  Spurious IRQ Flag. */
extern uint32_t IntSpurIrqFlagGet(void);

/* \brief   Reads the spurious FIQ Flag. Spurious FIQ flag is reflected in both
 *          SIR_FIQ and FIQ_PRIORITY registers of the interrupt controller.
 * \return  Spurious IRQ Flag. */
extern uint32_t IntSpurFiqFlagGet(void);

/* \brief   Returns the currently active IRQ priority level.
 * \return  Current IRQ priority */
extern uint32_t IntCurrIrqPriorityGet(void);

/* \brief   Returns the currently active FIQ priority level.
 * \return  Current FIQ priority */
extern uint32_t IntCurrFiqPriorityGet(void);

/* \brief   Returns the priority threshold.
 * \return  Priority threshold value. */
extern uint32_t IntPriorityThresholdGet(void);

/* \brief   Returns the raw interrupt status before masking.
 * \param   intrNum - the system interrupt number.
 * \return  TRUE - if the raw status is set
 *          FALSE - if the raw status is not set. */
extern uint32_t IntRawStatusGet(uint32_t intrNum);

/* \brief   Returns the IRQ status after masking.
 * \param   intrNum - the system interrupt number
 * \return  TRUE - if interrupt is pending
 *          FALSE - in no interrupt is pending */
extern uint32_t IntPendingIrqMaskedStatusGet(uint32_t intrNum);

/* \brief   Returns the FIQ status after masking.
 * \param   intrNum - the system interrupt number
 * \return  TRUE - if interrupt is pending
 *          FALSE - in no interrupt is pending */
extern uint32_t IntPendingFiqMaskedStatusGet(uint32_t intrNum);

#ifdef __cplusplus
}
#endif

#endif
