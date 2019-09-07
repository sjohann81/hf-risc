ENTRY(_entry)

/* specify HF-RISC memory areas */

MEMORY
{
rom		: ORIGIN = 0x00000000, LENGTH = 256M	/* ROM area*/
ram		: ORIGIN = 0x30000000, LENGTH = 256M	/* RAM area. */
}

/* define a global symbol _stack  */

_stack = 0x3001fffc;

/* 0x30000000 - ext SPI cs1, sram 3 byte address
   0x38000000 - ext SPI cs1, sram 3 byte address (uncached)
   0x33000000 - ext SPI cs2, eeprom 2 byte address
   0x3b000000 - ext SPI cs2, eeprom 2 byte address (uncached)
*/

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

