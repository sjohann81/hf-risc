	.text
	.align 2

	.global _entry
	.ent	_entry
_entry:
	.set noreorder

	la	$sp, _stack

	jal	main
	nop
$L1:
	beq	$zero, $zero, $L1
	nop
.end _entry

