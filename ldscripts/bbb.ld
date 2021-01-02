OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

/* The memory map */
MEMORY {
    SRAM :     o = 0x402F0400,  l = 0x0000FC00  /* 64KB internal SRAM */
    L3OCMC0 :  o = 0x40300000,  l = 0x00010000  /* 64KB L3 OCMC SRAM */
    M3SHUMEM : o = 0x44D00000,  l = 0x00004000  /* 16KB M3 Shared Unified Code Space */
    M3SHDMEM : o = 0x44D80000,  l = 0x00002000  /*  8KB M3 Shared Data Memory */
    DDR0 :     o = 0x80000000,  l = 0x00f00000 	/* 15MB on external DDR Bank 0 */
    DDR0_dma : o = 0x80f00000,  l = 0x00100000 	/*  1MB on external DDR Bank 0 */
}

SECTIONS
{
    .bootargs (NOLOAD):
    {
		. = . + 0x1000;

    } > DDR0

	.text : 
	{
		*(.text_startup)
		*(.text*)   

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        KEEP(*(.eh_frame*))

	} > DDR0

	.rodata : 
	{ 
		. = ALIGN(0x100);
		*(.rodata*) 
	} >DDR0
  
	. = ALIGN(0x100);
	.ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > DDR0

	__exidx_start = .;
	.ARM.exidx : 
	{ 
		*(.ARM.exidx* .gnu.linkonce.armexidx.*) 
	} > DDR0
	__exidx_end = .;

    .shell_cmd :
    {
         __shell_cmd_start = .;
       KEEP(*(.shell_cmd))
        __shell_cmd_end = .;
    } > DDR0

	.data : 
	{ 
		. = ALIGN(0x100);
		*(.data*)

        . = ALIGN(8);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(8);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(8);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        /* All data end */
        . = ALIGN(32);
	} >DDR0

    .uninitialized (NOLOAD):
    {
        . = ALIGN(32);
        __uninitialized_start = .;
        *(.uninitialized)
        KEEP(*(.keep.uninitialized))
        . = ALIGN(32);
        __uninitialized_end = .;
    } > DDR0

	.bss (NOLOAD):
	{
		. = ALIGN (0x100);
		__bss_start__ = . ;
		*(.bss*) 
		*(COMMON)
		. = ALIGN (0x100);
		__bss_end__ = .;
	} >DDR0

	_end = ALIGN(0x100);
	_stack_bottom = ORIGIN(DDR0) + LENGTH(DDR0) - 0x80000;
	__c_stack_top__ = ORIGIN(DDR0) + LENGTH(DDR0);  

	.dmasafe (NOLOAD) :
	{
		*(.dmasafe) 
	} >DDR0_dma
} 
