	.text
	.align 2

	.global _entry
_entry:
	la	sp, __stacktop

	# jump to main
	jal	ra, main

L1:
	beq	zero, zero, L1
