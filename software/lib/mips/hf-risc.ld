ENTRY(_entry)

/* specify HF-RISC memory areas */

MEMORY 
{
rom		: ORIGIN = 0x00000000, LENGTH = 256M	/* ROM area */
ram		: ORIGIN = 0x40000000, LENGTH = 256M	/* RAM area */
ram_top		: ORIGIN = 0x4ffffc00, LENGTH = 0x400	/* RAM area, 1KB reserved for the bootloader .bss + stack */
}

/* define a global symbol _stack  */

_stack = 0x4ffffffc;

/* now define the output sections  */

SECTIONS 
{
	. = 0;

	.text :
	{
		*(.text.init)
	}
	
	.text :
	{
		_text = .;
		*(.text)
	        *(.text.*)
		_etext = .;
	} > ram

	.rodata :
	{
		_rodata = .;
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
	} > ram

	.data :
	{
		_data = .;
		*(.data)
		*(.data.*)
		_gp = . + 0x7ff0;
		*(.sdata)
		*(.sdata.*)
		. = ALIGN(16);
	} > ram

	_edata = .;

	.bss :
	{
		_bss_start = .;
		*(.sbss) 
		*(.sbss.*)
		*(.bss) 
		*(.bss.*)
		. = ALIGN(4);
		_bss_end = . ;
	} > ram

}
	_end = .;

