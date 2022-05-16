/* HF-RISCV memory areas */

MEMORY
{
	RAM (rwx)     : ORIGIN = 0x40000000, LENGTH = 32K
}

/* Entry Point */
ENTRY(_entry)

/* Defines beginning and ending of stack. */
_stack_size  = 8K;
_stack = _stack_start;
_stack_start = ORIGIN(RAM) + LENGTH(RAM);
_stack_end   = _stack_start - _stack_size;

/* Defines beginning and ending of heap. */
_heap_start = _ebss;
_heap_end   = _stack_end;
_heap_size  = _stack_end - _ebss;

/* Describes the placement of each output section, including the input sections which are inserted into them */
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
	} > RAM

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
	} > RAM

	_edata = .;

	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		_sbss = .;
		*(.sbss)
		*(.sbss.*)
		*(.bss)
		*(.bss.*)
		. = ALIGN(4);
		_ebss = .;
	} > RAM
	
	_end = .;

	.heap . (NOLOAD) :
	{
		. = ALIGN(4);
		_heap_start = .;
		. = . + _heap_size;
		. = ALIGN(4);
	} > RAM

	.stack . (NOLOAD) :
	{
		. = ALIGN(4);
		_stack_end = .;
		. = . + _stack_size;
		_stack_start = .;
		. = ALIGN(4);
	} > RAM
}
