OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

/* The memory map */
MEMORY {
    DDR0 : o = 0x20000000,  l = 0x00100000 	/* 1MB external DDR Bank 0 */
}

SECTIONS
{
	.text : 
	{
		_base_args = . ;
		. = . + 0x1000;
		*(.text_startup)
		*(.text*)   
	} > DDR0

	.rodata : 
	{ 
		. = ALIGN(0x100);
		*(.rodata*) 
	} >DDR0

	.data : 
	{ 
		. = ALIGN(0x100);
		*(.data*)
	} >DDR0
  
	. = ALIGN(0x100);
	__exidx_start = .;
	.ARM.exidx : 
	{ 
		*(.ARM.exidx* .gnu.linkonce.armexidx.*) 
	} > DDR0
	__exidx_end = .;
  
	.bss :
	{
		. = ALIGN (0x100);
		__bss_start__ = . ;
		*(.bss*) 
		*(COMMON)
		. = ALIGN (0x100);
		__bss_end__ = .;
	} >DDR0

	_end = ALIGN(0x100);
	_stack_bottom = ORIGIN(DDR0) + LENGTH(DDR0) - 0x00008000;
	__c_stack_top__ = ORIGIN(DDR0) + LENGTH(DDR0) - 0x100;  
} 
