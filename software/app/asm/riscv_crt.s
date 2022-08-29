	.text
	.align 2

	.global main
main:
	li t0,0xf00000d0
	addi t1,zero,'!'
	sw t1,0(t0)
	li	t0, 0xe0000000		# this will interrupt the simulation (assertion)
	sw	zero, 0(t0)

	ret
