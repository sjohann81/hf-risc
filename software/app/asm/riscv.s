	.text
	.align 2

	.global _entry
_entry:
	la	a3, _sbss
	la	a2, _ebss
	la	gp, _gp
	la	sp, _stack
	la	tp, _end + 63
	and	tp, tp, -64

BSS_CLEAR:
	# clear the .bss
	sw	zero, 0(a3)
	addi	a3, a3, 4
	blt	a3, a2, BSS_CLEAR

	# jump to main
	jal	ra, main

	li	t0, 0xe0000000		# this will interrupt the simulation (assertion)
	sw	a0,0(t0)
L1:
	beq	zero, zero, L1


	.global main
main:
	li t0,0xf00000d0
	addi t1,zero,'!'
	sw t1,0(t0)
	
	ret
