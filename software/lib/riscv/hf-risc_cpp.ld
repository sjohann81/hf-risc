ENTRY(_start)

/* specify HF-RISC memory areas */

MEMORY
{
rom		: ORIGIN = 0x00000000, LENGTH = 1M	/* ROM area*/
ram		: ORIGIN = 0x40000000, LENGTH = 1M	/* RAM area. */
ram_top		: ORIGIN = 0x4ffffc00, LENGTH = 0x400	/* RAM area. 1KB reserved for the bootloader .bss + stack */
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

	.ctors :
	{
		PROVIDE(__ctors_start__ = .);
		KEEP(*(SORT(.ctors.*)))
		KEEP(*(.ctors))
		PROVIDE(__ctors_end__ = .);
	} > ram

	.dtors :
	{
		PROVIDE(__dtors_start__ = .);
		KEEP(*(SORT(.dtors.*)))
		KEEP(*(.dtors))
		PROVIDE(__dtors_end__ = .);
	} > ram

	.data :
	{
		_rodata = .;
		*(.rdata)
		*(.rodata)
		*(.rodata.*)
		_erodata = .;
		_data = .;
		*(.data)
		*(.data.*)
		__global_pointer$ = . + 0x800;
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

	_end = .;
}

