/*  \file   interrupt.c
 *  \brief  Interrupt related APIs.
 *   This file contains the APIs for configuring AINTC */
/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ */
/*
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include "am335x_irq.h"

#include <stdint.h>

/*************************************************************************\
 * Registers Definition
\************************************************************************/
/** @brief Base address of AINTC memory mapped registers                      */
#define SOC_AINTC_REGS                          (0x48200000)
#define INTC_REVISION                           (0x0)
#define INTC_SYSCONFIG                          (0x10)
#define INTC_SYSSTATUS                          (0x14)
#define INTC_SIR_IRQ                            (0x40)
#define INTC_SIR_FIQ                            (0x44)
#define INTC_CONTROL                            (0x48)
#define INTC_PROTECTION                         (0x4c)
#define INTC_IDLE                               (0x50)
#define INTC_IRQ_PRIORITY                       (0x60)
#define INTC_FIQ_PRIORITY                       (0x64)
#define INTC_THRESHOLD                          (0x68)
#define INTC_SICR                               (0x6c)
#define INTC_SCR(n)                             (0x70 + ((n)*0x04))
#define INTC_ITR(n)                             (0x80 + ((n)*0x20))
#define INTC_MIR(n)                             (0x84 + ((n)*0x20))
#define INTC_MIR_CLEAR(n)                       (0x88 + ((n)*0x20))
#define INTC_MIR_SET(n)                         (0x8c + ((n)*0x20))
#define INTC_ISR_SET(n)                         (0x90 + ((n)*0x20))
#define INTC_ISR_CLEAR(n)                       (0x94 + ((n)*0x20))
#define INTC_PENDING_IRQ(n)                     (0x98 + ((n)*0x20))
#define INTC_PENDING_FIQ(n)                     (0x9c + ((n)*0x20))
#define INTC_ILR(n)                             (0x100 + ((n)*0x04))
/**************************************************************************\
* Field Definition Macros
\*************************************************************************/
/* REVISION */
#define INTC_REVISION_REV                       (0x000000FFu)
#define INTC_REVISION_REV_SHIFT                 (0x00000000u)
/* SYSCONFIG */
#define INTC_SYSCONFIG_SOFTRESET                (0x00000002u)
#define INTC_SYSCONFIG_SOFTRESET_SHIFT          (0x00000001u)
#define INTC_SYSCONFIG_AUTOIDLE                 (0x00000001u)
#define INTC_SYSCONFIG_AUTOIDLE_SHIFT           (0x00000000u)
/* SYSSTATUS */
#define INTC_SYSSTATUS_RESETDONE                (0x00000001u)
#define INTC_SYSSTATUS_RESETDONE_SHIFT          (0x00000000u)
/* SIR_IRQ */
#define INTC_SIR_IRQ_SPURIOUSIRQ                (0xFFFFFF80u)
#define INTC_SIR_IRQ_SPURIOUSIRQ_SHIFT          (0x00000007u)
#define INTC_SIR_IRQ_ACTIVEIRQ                  (0x0000007F)
#define INTC_SIR_IRQ_ACTIVEIRQ_SHIFT            (0x00000000)
/* SIR_FIQ */
#define INTC_SIR_FIQ_SPURIOUSFIQ                (0xFFFFFF80)
#define INTC_SIR_FIQ_SPURIOUSFIQ_SHIFT          (0x00000007)
#define INTC_SIR_FIQ_ACTIVEFIQ                  (0x0000007F)
#define INTC_SIR_FIQ_ACTIVEFIQ_SHIFT            (0x00000000)
/* CONTROL */
#define INTC_CONTROL_NEWFIQAGR                  (0x00000002)
#define INTC_CONTROL_NEWFIQAGR_SHIFT            (0x00000001)
#define INTC_CONTROL_NEWIRQAGR                  (0x00000001)
#define INTC_CONTROL_NEWIRQAGR_SHIFT            (0x00000000)
/* PROTECTION */
#define INTC_PROTECTION_PROTECTION              (0x00000001u)
#define INTC_PROTECTION_PROTECTION_SHIFT        (0x00000000u)
/* IDLE */
#define INTC_IDLE_TURBO                         (0x00000002u)
#define INTC_IDLE_TURBO_SHIFT                   (0x00000001u)
#define INTC_IDLE_FUNCIDLE                      (0x00000001u)
#define INTC_IDLE_FUNCIDLE_SHIFT                (0x00000000u)
/* IRQ_PRIORITY */
#define INTC_IRQ_PRIORITY_SPURIOUSIRQFLAG       (0xFFFFFFC0u)
#define INTC_IRQ_PRIORITY_SPURIOUSIRQFLAG_SHIFT (0x00000006u)
#define INTC_IRQ_PRIORITY_IRQPRIORITY           (0x0000003Fu)
#define INTC_IRQ_PRIORITY_IRQPRIORITY_SHIFT     (0x00000000u)
/* FIQ_PRIORITY */
#define INTC_FIQ_PRIORITY_SPURIOUSFIQFLAG       (0xFFFFFFC0u)
#define INTC_FIQ_PRIORITY_SPURIOUSFIQFLAG_SHIFT (0x00000006u)
#define INTC_FIQ_PRIORITY_FIQPRIORITY           (0x0000003Fu)
#define INTC_FIQ_PRIORITY_FIQPRIORITY_SHIFT     (0x00000000u)
/* THRESHOLD */
#define INTC_THRESHOLD_PRIORITYTHRESHOLD        (0x000000FFu)
#define INTC_THRESHOLD_PRIORITYTHRESHOLD_SHIFT  (0x00000000u)
/* SICR */
#define INTC_SICR_GLOBALMASK                    (0x00000040u)
#define INTC_SICR_GLOBALMASK_SHIFT              (0x00000006u)
#define INTC_SICR_SOFTRESETINH                  (0x00000020u)
#define INTC_SICR_SOFTRESETINH_SHIFT            (0x00000005u)
#define INTC_SICR_PUBLICMASKFEEDBACK            (0x00000010u)
#define INTC_SICR_PUBLICMASKFEEDBACK_SHIFT      (0x00000004u)
#define INTC_SICR_PUBLICINHIBIT                 (0x00000008u)
#define INTC_SICR_PUBLICINHIBIT_SHIFT           (0x00000003u)
#define INTC_SICR_AUTOINHIBIT                   (0x00000004u)
#define INTC_SICR_AUTOINHIBIT_SHIFT             (0x00000002u)
#define INTC_SICR_SSMFIQENABLE                  (0x00000002u)
#define INTC_SICR_SSMFIQENABLE_SHIFT            (0x00000001u)
#define INTC_SICR_SSMFIQSTATUS                  (0x00000001u)
#define INTC_SICR_SSMFIQSTATUS_SHIFT            (0x00000000u)
/* SCR0 */
#define INTC_SCR0_SECUREENABLE                  (0xFFFFFFFFu)
#define INTC_SCR0_SECUREENABLE_SHIFT            (0x00000000u)
/* SCR1 */
#define INTC_SCR1_SECUREENABLE                  (0xFFFFFFFFu)
#define INTC_SCR1_SECUREENABLE_SHIFT            (0x00000000u)
/* SCR2 */
#define INTC_SCR2_SECUREENABLE                  (0xFFFFFFFFu)
#define INTC_SCR2_SECUREENABLE_SHIFT            (0x00000000u)
/* ITR0 */
#define INTC_ITR0_ITR                           (0xFFFFFFFFu)
#define INTC_ITR0_ITR_SHIFT                     (0x00000000u)
/* MIR0 */
#define INTC_MIR0_MIR                           (0xFFFFFFFFu)
#define INTC_MIR0_MIR_SHIFT                     (0x00000000u)
/* MIR_CLEAR0 */
#define INTC_MIR_CLEAR0_MIRCLEAR                (0xFFFFFFFFu)
#define INTC_MIR_CLEAR0_MIRCLEAR_SHIFT          (0x00000000u)
/* MIR_SET0 */
#define INTC_MIR_SET0_MIRSET                    (0xFFFFFFFFu)
#define INTC_MIR_SET0_MIRSET_SHIFT              (0x00000000u)
/* ISR_SET0 */
#define INTC_ISR_SET0_ISRSET                    (0xFFFFFFFFu)
#define INTC_ISR_SET0_ISRSET_SHIFT              (0x00000000u)
/* ISR_CLEAR0 */
#define INTC_ISR_CLEAR0_ISRCLEAR                (0xFFFFFFFFu)
#define INTC_ISR_CLEAR0_ISRCLEAR_SHIFT          (0x00000000u)
/* PENDING_IRQ0 */
#define INTC_PENDING_IRQ0_PENDING_IRQ           (0xFFFFFFFFu)
#define INTC_PENDING_IRQ0_PENDING_IRQ_SHIFT     (0x00000000u)
/* PENDING_FIQ0 */
#define INTC_PENDING_FIQ0_PENDING_FIQ           (0xFFFFFFFFu)
#define INTC_PENDING_FIQ0_PENDING_FIQ_SHIFT     (0x00000000u)
/* ITR1 */
#define INTC_ITR1_ITR                           (0xFFFFFFFFu)
#define INTC_ITR1_ITR_SHIFT                     (0x00000000u)
/* MIR1 */
#define INTC_MIR1_MIR                           (0xFFFFFFFFu)
#define INTC_MIR1_MIR_SHIFT                     (0x00000000u)
/* MIR_CLEAR1 */
#define INTC_MIR_CLEAR1_MIRCLEAR                (0xFFFFFFFFu)
#define INTC_MIR_CLEAR1_MIRCLEAR_SHIFT          (0x00000000u)
/* MIR_SET1 */
#define INTC_MIR_SET1_MIRSET                    (0xFFFFFFFFu)
#define INTC_MIR_SET1_MIRSET_SHIFT              (0x00000000u)
/* ISR_SET1 */
#define INTC_ISR_SET1_ISRSET                    (0xFFFFFFFFu)
#define INTC_ISR_SET1_ISRSET_SHIFT              (0x00000000u)
/* ISR_CLEAR1 */
#define INTC_ISR_CLEAR1_ISRCLEAR                (0xFFFFFFFFu)
#define INTC_ISR_CLEAR1_ISRCLEAR_SHIFT          (0x00000000u)
/* PENDING_IRQ1 */
#define INTC_PENDING_IRQ1_PENDING_IRQ           (0xFFFFFFFFu)
#define INTC_PENDING_IRQ1_PENDING_IRQ_SHIFT     (0x00000000u)
/* PENDING_FIQ1 */
#define INTC_PENDING_FIQ1_PENDING_FIQ           (0xFFFFFFFFu)
#define INTC_PENDING_FIQ1_PENDING_FIQ_SHIFT     (0x00000000u)
/* ITR2 */
#define INTC_ITR2_ITR                           (0xFFFFFFFFu)
#define INTC_ITR2_ITR_SHIFT                     (0x00000000u)
/* MIR2 */
#define INTC_MIR2_MIR                           (0xFFFFFFFFu)
#define INTC_MIR2_MIR_SHIFT                     (0x00000000u)
/* MIR_CLEAR2 */
#define INTC_MIR_CLEAR2_MIRCLEAR                (0xFFFFFFFFu)
#define INTC_MIR_CLEAR2_MIRCLEAR_SHIFT          (0x00000000u)
/* MIR_SET2 */
#define INTC_MIR_SET2_MIRSET                    (0xFFFFFFFFu)
#define INTC_MIR_SET2_MIRSET_SHIFT              (0x00000000u)
/* ISR_SET2 */
#define INTC_ISR_SET2_ISRSET                    (0xFFFFFFFFu)
#define INTC_ISR_SET2_ISRSET_SHIFT              (0x00000000u)
/* ISR_CLEAR2 */
#define INTC_ISR_CLEAR2_ISRCLEAR                (0xFFFFFFFFu)
#define INTC_ISR_CLEAR2_ISRCLEAR_SHIFT          (0x00000000u)
/* PENDING_IRQ2 */
#define INTC_PENDING_IRQ2_PENDING_IRQ           (0xFFFFFFFFu)
#define INTC_PENDING_IRQ2_PENDING_IRQ_SHIFT     (0x00000000u)
/* PENDING_FIQ2 */
#define INTC_PENDING_FIQ2_PENDING_FIQ           (0xFFFFFFFFu)
#define INTC_PENDING_FIQ2_PENDING_FIQ_SHIFT     (0x00000000u)
/* ILR */
#define INTC_ILR_PRIORITY                       (0x000001FCu)
#define INTC_ILR_PRIORITY_SHIFT                 (0x00000002u)
#define INTC_ILR_FIQNIRQ                        (0x00000001u)
#define INTC_ILR_FIQNIRQ_SHIFT                  (0x00000000u)

#define REG_IDX_SHIFT                           (0x05)
#define REG_BIT_MASK                            (0x1F)
#define NUM_INTERRUPTS                          (128u)

/******************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/

#define HWREG(x)                                (*((volatile uint32_t *)(x)))
#define HWREGH(x)                               (*((volatile uint16_t *)(x)))
#define HWREGB(x)                               (*((volatile uint8_t *)(x)))

#define HWREGBITW(x, b)                             \
    HWREG(((uint32_t)(x)&0xF0000000) | 0x02000000 | \
          (((uint32_t)(x)&0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITH(x, b)                              \
    HWREGH(((uint32_t)(x)&0xF0000000) | 0x02000000 | \
           (((uint32_t)(x)&0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITB(x, b)                              \
    HWREGB(((uint32_t)(x)&0xF0000000) | 0x02000000 | \
           (((uint32_t)(x)&0x000FFFFF) << 5) | ((b) << 2))

#define TRUE  1
#define FALSE 0

/**************** *************************************************************
**                 STATIC VARIABLE DEFINITIONS
*****************************************************************************/
static void (*fnRAMVectors[NUM_INTERRUPTS])(void);

/******************************************************************************
**                     API FUNCTION DEFINITIONS
*****************************************************************************/
/* The Default Interrupt Handler.
 * This is the default interrupt handler for all interrupts.  It simply loops
 * forever so that the system state is preserved for observation by a
 * debugger.  Since interrupts should be disabled before unregistering the
 * corresponding handler, this should never be called. */
static void IntDefaultHandler(void) {
    while (1) continue;
}

void IntRegister(uint32_t intrNum, void (*fnHandler)(void)) {
    fnRAMVectors[intrNum] = fnHandler;
}

void IntUnRegister(uint32_t intrNum) {
    fnRAMVectors[intrNum] = IntDefaultHandler;
}

void IntAINTCInit(void) {
    /* Reset the ARM interrupt controller */
    HWREG(SOC_AINTC_REGS + INTC_SYSCONFIG) = INTC_SYSCONFIG_SOFTRESET;

    /* Wait for the reset to complete */
    while ((HWREG(SOC_AINTC_REGS + INTC_SYSSTATUS) &
            INTC_SYSSTATUS_RESETDONE) != INTC_SYSSTATUS_RESETDONE)
        ;

    /* Enable any interrupt generation by setting priority threshold */
    HWREG(SOC_AINTC_REGS + INTC_THRESHOLD) = INTC_THRESHOLD_PRIORITYTHRESHOLD;
}

void IntPrioritySet(uint32_t intrNum, uint32_t priority, uint32_t hostIntRoute) {
    HWREG(SOC_AINTC_REGS + INTC_ILR(intrNum)) =
        ((priority << INTC_ILR_PRIORITY_SHIFT) & INTC_ILR_PRIORITY) |
        hostIntRoute;
}

void IntSystemEnable(uint32_t intrNum) {
    /* Disable the system interrupt in the corresponding MIR_CLEAR register */
    HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR(intrNum >> REG_IDX_SHIFT)) =
        (0x01 << (intrNum & REG_BIT_MASK));
}

void IntSystemDisable(uint32_t intrNum) {
    HWREG(SOC_AINTC_REGS + INTC_MIR_SET(intrNum >> REG_IDX_SHIFT)) =
        (0x01 << (intrNum & REG_BIT_MASK));
}

void IntIfClkFreeRunSet(void) {
    HWREG(SOC_AINTC_REGS + INTC_SYSCONFIG) &= ~INTC_SYSCONFIG_AUTOIDLE;
}

void IntIfClkAutoGateSet(void) {
    HWREG(SOC_AINTC_REGS + INTC_SYSCONFIG) |= INTC_SYSCONFIG_AUTOIDLE;
}

uint32_t IntActiveIrqNumGet(void) {
    return (HWREG(SOC_AINTC_REGS + INTC_SIR_IRQ) & INTC_SIR_IRQ_ACTIVEIRQ);
}

uint32_t IntActiveFiqNumGet(void) {
    return (HWREG(SOC_AINTC_REGS + INTC_SIR_FIQ) & INTC_SIR_FIQ_ACTIVEFIQ);
}

uint32_t IntSpurIrqFlagGet(void) {
    return ((HWREG(SOC_AINTC_REGS + INTC_SIR_IRQ) & INTC_SIR_IRQ_SPURIOUSIRQ) >>
            INTC_SIR_IRQ_SPURIOUSIRQ_SHIFT);
}

uint32_t IntSpurFiqFlagGet(void) {
    return ((HWREG(SOC_AINTC_REGS + INTC_SIR_FIQ) & INTC_SIR_FIQ_SPURIOUSFIQ) >>
            INTC_SIR_FIQ_SPURIOUSFIQ_SHIFT);
}

void IntProtectionEnable(void) {
    HWREG(SOC_AINTC_REGS + INTC_PROTECTION) = INTC_PROTECTION_PROTECTION;
}

void IntProtectionDisable(void) {
    HWREG(SOC_AINTC_REGS + INTC_PROTECTION) &= ~INTC_PROTECTION_PROTECTION;
}

void IntSyncClkFreeRunSet(void) {
    HWREG(SOC_AINTC_REGS + INTC_IDLE) &= ~INTC_IDLE_TURBO;
}

void IntSyncClkAutoGateSet(void) {
    HWREG(SOC_AINTC_REGS + INTC_IDLE) |= INTC_IDLE_TURBO;
}

void IntFuncClkFreeRunSet(void) {
    HWREG(SOC_AINTC_REGS + INTC_IDLE) |= INTC_IDLE_FUNCIDLE;
}

void IntFuncClkAutoGateSet(void) {
    HWREG(SOC_AINTC_REGS + INTC_IDLE) &= ~INTC_IDLE_FUNCIDLE;
}

uint32_t IntCurrIrqPriorityGet(void) {
    return (HWREG(SOC_AINTC_REGS + INTC_IRQ_PRIORITY) &
            INTC_IRQ_PRIORITY_IRQPRIORITY);
}

uint32_t IntCurrFiqPriorityGet(void) {
    return (HWREG(SOC_AINTC_REGS + INTC_FIQ_PRIORITY) &
            INTC_FIQ_PRIORITY_FIQPRIORITY);
}

uint32_t IntPriorityThresholdGet(void) {
    return (HWREG(SOC_AINTC_REGS + INTC_THRESHOLD) &
            INTC_THRESHOLD_PRIORITYTHRESHOLD);
}

void IntPriorityThresholdSet(uint32_t threshold) {
    HWREG(SOC_AINTC_REGS + INTC_THRESHOLD) =
        threshold & INTC_THRESHOLD_PRIORITYTHRESHOLD;
}

uint32_t IntRawStatusGet(uint32_t intrNum) {
    return (
        (0 == ((HWREG(SOC_AINTC_REGS + INTC_ITR(intrNum >> REG_IDX_SHIFT)) >>
                (intrNum & REG_BIT_MASK)) &
               0x01))
            ? FALSE
            : TRUE);
}

void IntSoftwareIntSet(uint32_t intrNum) {
    HWREG(SOC_AINTC_REGS + INTC_ISR_SET(intrNum >> REG_IDX_SHIFT)) =
        (0x01 << (intrNum & REG_BIT_MASK));
}

void IntSoftwareIntClear(uint32_t intrNum) {
    HWREG(SOC_AINTC_REGS + INTC_ISR_CLEAR(intrNum >> REG_IDX_SHIFT)) =
        (0x01 << (intrNum & REG_BIT_MASK));
}

uint32_t IntPendingIrqMaskedStatusGet(uint32_t intrNum) {
    return ((0 == (HWREG(SOC_AINTC_REGS +
                         INTC_PENDING_IRQ(intrNum >> REG_IDX_SHIFT)) >>
                   (((intrNum & REG_BIT_MASK)) & 0x01)))
                ? FALSE
                : TRUE);
}

uint32_t IntPendingFiqMaskedStatusGet(uint32_t intrNum) {
    return ((0 == (HWREG(SOC_AINTC_REGS +
                         INTC_PENDING_FIQ(intrNum >> REG_IDX_SHIFT)) >>
                   (((intrNum & REG_BIT_MASK)) & 0x01)))
                ? FALSE
                : TRUE);
}

void IntMasterIRQEnable(void) {
    CPUirqe();
}

void IntMasterIRQDisable(void) {
    CPUirqd();
}

void IntMasterFIQEnable(void) {
    CPUfiqe();
}

void IntMasterFIQDisable(void) {
    CPUfiqd();
}

uint32_t IntMasterStatusGet(void) { 
    return CPUIntStatus();
}

uint8_t IntDisable(void) {
    uint8_t status;
    /* Reads the current status.*/
    status = (IntMasterStatusGet() & 0xFF);
    /* Disable the Interrupts.*/
    IntMasterIRQDisable();
    return status;
}

void IntEnable(uint8_t status) {
    if ((status & 0x80) == 0) IntMasterIRQEnable();
}
