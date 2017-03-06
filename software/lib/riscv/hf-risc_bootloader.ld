ENTRY(_entry)

/* specify HF-RISC memory areas */

MEMORY 
{
rom		: ORIGIN = 0x00000000, LENGTH = 256M	/* ROM area*/
ram		: ORIGIN = 0x40000000, LENGTH = 256M	/* RAM area. */
ram_top		: ORIGIN = 0x4ffffc00, LENGTH = 0x400	/* RAM area. 1KB reserved for the bootloader .bss + stack */
}

/* define a global symbol _stack on top of RAM. */

_stack = 0x4ffffffc;

/* now define the output sections  */

SECTIONS 
{
	. = 0;

	.text :
	{
		_text = .;
		*(.text)
	        *(.text.*)
		_etext = .;
	} > rom

	.rodata :
	{
		_rodata = .;
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
	} > rom
/* TODO: copy .data to RAM */
/* now, we just don't generate globals, so we can safely remove the .data section */
/*
	.data :
	{
		_data = .;
		*(.data)
		*(.data.*)
		_edata = .;
	} > ram
*/
/* the .bss section must be defined in RAM. we don't need to initialize it though. */
	.bss :
	{
		_bss_start = .;
		*(.bss)
	} > ram_top

	. = ALIGN(4);
	_bss_end = . ;
}
	_end = .;

