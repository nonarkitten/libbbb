/**
 * Copyright 2014 University of Applied Sciences Western Switzerland / Fribourg
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
 * Project:	HEIA-FR / Embedded Systems 1+2 Laboratory
 *
 * Abstract:	AM335x startup 
 *
 * Purpose:	BeagleBone Black - AM335x startup code
 *
 * Author: 	Daniel Gachet
 * Date: 	24.05.2015
 */

/* base code: TI - ---> more details soon...
 * source file: /home/lmi/workspace/bbb/bbdrv/system_config/armv7a/gcc/init.S 
 */


	.bss
stack:	.space 0x1000
stacktop:
	.text
	.global _start
	.section .text_startup

_start:	nop
	ldr	lr, =end  /* init lr to avoid errmsg on gdb */


	// to be adapted....
	msr	cpsr_c, #0xdf
	ldr	sp,=stacktop
	msr	cpsr_c, #0xd1
	ldr	sp,=_stack_bottom + 0x10
	msr	cpsr_c, #0xd2
	ldr	sp,=_stack_bottom + 0x18
	msr	cpsr_c, #0xd7
	ldr	sp,=_stack_bottom + 0x20
	msr	cpsr_c, #0xdb
	ldr	sp,=_stack_bottom + 0x28


	/* set the C stack pointer */
	/* Some info about GDB & OpenOCD 
	 * (faq: http://openocd.berlios.de/doc/html/FAQ.html) 
	 * push 8 registers of zeros onto the stack before calling main(). 
	 * What GDB is doing is “climbing” the run time stack by reading 		 
	 * various values on the stack using the standard call frame for 
	 * the target. GDB keeps going - until one of 2 things happen #1 
	 * an invalid frame is found, or #2 some huge number of stackframes
	 * have been processed. By pushing zeros on the stack, GDB 
	 * gracefully stops.
 	 */
	msr	cpsr_c, #0xd3
	ldr	sp,=__c_stack_top__
	mov	r0, #0			
	mov	r1, #8
fill:	str	r0, [sp, #-4]!
	subs	r1, #1
	bne	fill


	// Invalidate and Enable Branch Prediction  
	mov     r0, #0
	mcr     p15, #0, r0, c7, c5, #6
	isb
	mrc     p15, #0, r0, c1, c0, #0
	orr     r0, r0, #0x00000800
	mcr     p15, #0, r0, c1, c0, #0

	// Enable Neon/VFP Co-Processor
	mrc 	p15, #0, r1, c1, c0, #2		// r1 = Access Control Register
	orr 	r1, r1, #(0xf << 20)		// enable full access for p10,11
	mcr 	p15, #0, r1, c1, c0, #2		// Access Control Register = r1
	mov 	r1, #0
	mcr 	p15, #0, r1, c7, c5, #4		// flush prefetch buffer
	mov 	r0,#0x40000000
	fmxr 	fpexc, r0			// Set Neon/VFP Enable bit


	// disable am335x watchdog
	ldr	r0, =0x44e35000			// load SOC_WDT_1_REGS
	ldr	r1, =0xaaaa
	str	r1, [r0, #0x48]			// store 0xaaaa to WDT_WSPR
1:	ldr	r1, [r0, #0x34]			// loop until WDT_WWPS is 0
	cmp	r1, #0x0
	bne	1b
	ldr	r1, =0x5555
	str	r1, [r0, #0x48]			// store 0x5555 to WDT_WSPR
2:	ldr	r1, [r0, #0x34]			// loop until WDT_WWPS is 0
	cmp	r1, #0x0
	bne	2b

	// allow unaligned access, effective only when MMU is enabled
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, #0x2
	mcr	p15, 0, r0, c1, c0, 0


        // Clear the .bss section (zero init)
	mov	r0, #0
        ldr	r1, =__bss_start__
        ldr	r2, =_end
loop:	cmp	r1, r2
	strlo	r0, [r1], #4
        blo     loop
        
	// Initialize stdlib
	ldr	r0,=_init
	blx	r0

	// Enter main program
	ldr	r0, =main
        blx	r0

end:	nop
	b	end
