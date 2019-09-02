ENTRY(_entry)

/* specify HF-RISC memory areas */

MEMORY
{
rom		: ORIGIN = 0x00000000, LENGTH = 256M
sram		: ORIGIN = 0x30000000, LENGTH = 128K
eeprom		: ORIGIN = 0x33000000, LENGTH = 32K
ram		: ORIGIN = 0x40000000, LENGTH = 256M
}

/* define a global symbol _stack  */

/* _stack = 0x6001fffc; */

_stack = 0x4001fffc;

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
	} > eeprom

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
		_gp = . + 0x800;
		*(.sdata)
		*(.sdata.*)
		. = ALIGN(16);
	} > eeprom

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

