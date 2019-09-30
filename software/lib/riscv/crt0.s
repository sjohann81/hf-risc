	.text
	.align 2

	.global _entry
_entry:
	la	a3, _bss_start
	la	a2, _end
	la	gp, _gp
	la	sp, _stack
	la	tp, _end + 63
	and	tp, tp, -64

BSS_CLEAR:
	# clear the .bss
	sw	zero, 0(a3)
	addi	a3, a3, 4
	blt	a3, a2, BSS_CLEAR

	# configure IRQ_VECTOR
	la	s11, _isr
	li	s10, 0xf0000000
	sw	s11, 0(s10)

	# enable global interrupts
	jal	ra, irq_enable

	# jump to main
	jal	ra, main

	li	s10, 0xe0000000		# this will interrupt the simulation (assertion)
	sw	a0,0(s10)
L1:
	beq	zero, zero, L1

	.org 0xe0
	.global _bootsignature
_bootsignature:
	.byte 0xb1
	.byte 0x6b
	.byte 0x00
	.byte 0xb5

# interrupt / exception service routine
	.org 0x100
	.global _isr
_isr:
	nop				# this must be a NOP
	addi	sp, sp, -68
	sw	ra, 0(sp)
	sw	t0, 4(sp)
	sw	t1, 8(sp)
	sw	t2, 12(sp)
	sw	a0, 16(sp)
	sw	a1, 20(sp)
	sw	a2, 24(sp)
	sw	a3, 28(sp)
	sw	a4, 32(sp)
	sw	a5, 36(sp)
	sw	a6, 40(sp)
	sw	a7, 44(sp)
	sw	t3, 48(sp)
	sw	t4, 52(sp)
	sw	t5, 56(sp)
	sw	t6, 60(sp)
	li	s10, 0xf0000040		# read IRQ_EPC
	lw	s10, 0(s10)
	addi	s10, s10, -4		# rollback, last opcode (at EPC) was not commited
	sw	s10, 64(sp)
	lui	a1, 0xf0000
	lw	a0, 0x10(a1)		# read IRQ_CAUSE
	lw	a2, 0x20(a1)		# read IRQ_MASK
	and	a0, a0, a2		# pass CAUSE and MASK and the stack pointer to the C handler
	addi	a1, sp, 0
	beq	a0, zero, _exception	# it's an exception, not an interrupt
	jal	ra, irq_handler		# jump to C handler
_restore:
	lw	a0, 16(sp)
	lw	a1, 20(sp)
_restore_exception:
	lw	ra, 0(sp)
	lw	t0, 4(sp)
	lw	t1, 8(sp)
	lw	t2, 12(sp)
	lw	a2, 24(sp)
	lw	a3, 28(sp)
	lw	a4, 32(sp)
	lw	a5, 36(sp)
	lw	a6, 40(sp)
	lw	a7, 44(sp)
	lw	t3, 48(sp)
	lw	t4, 52(sp)
	lw	t5, 56(sp)
	lw	t6, 60(sp)
	addi	sp, sp, 68
	ori	s11, zero, 0x1
	li	s10, 0xf0000030
	sw	s11, 0(s10)		# enable interrupts after a few cycles
	lw	s10, -4(sp)
	jalr	zero, s10		# context restored, continue
_exception:
	# pass syscall code and parameters to the exception handler
	addi	a0, a7, 0
	lw	a1, 20(sp)
	lw	a2, 24(sp)
	lw	a3, 28(sp)
	# call exception handler. return values will be at a0 and a1
	jal	ra, exception_handler
	jal	zero, _restore_exception

	.global _interrupt_set
_interrupt_set:
	li	a1, 0xf0000030
	lw	a2, 0(a1)
	sw	a0, 0(a1)
	addi	a0, a2, 0
	ret

	.global   setjmp
setjmp:
	sw    s0, 0(a0)
	sw    s1, 4(a0)
	sw    s2, 8(a0)
	sw    s3, 12(a0)
	sw    s4, 16(a0)
	sw    s5, 20(a0)
	sw    s6, 24(a0)
	sw    s7, 28(a0)
	sw    s8, 32(a0)
	sw    s9, 36(a0)
#	sw    gp, 40(a0)
	sw    tp, 44(a0)
	sw    sp, 48(a0)
	sw    ra, 52(a0)
	ori  a0, zero, 0
	ret

	.global   longjmp
longjmp:
	lw    s0, 0(a0)
	lw    s1, 4(a0)
	lw    s2, 8(a0)
	lw    s3, 12(a0)
	lw    s4, 16(a0)
	lw    s5, 20(a0)
	lw    s6, 24(a0)
	lw    s7, 28(a0)
	lw    s8, 32(a0)
	lw    s9, 36(a0)
#	lw    gp, 40(a0)
	lw    tp, 44(a0)
	lw    sp, 48(a0)
	lw    ra, 52(a0)
	ori  a0, a1, 0
	ret

# system call interface: syscall(service, arg0, arg1, arg2)
	.global syscall
syscall:
	addi	a7, a0, 0
	ecall
	ret
