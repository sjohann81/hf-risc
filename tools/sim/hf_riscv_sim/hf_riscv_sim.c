/* file:          hf_riscv_sim.c
 * description:   HF-RISCV simulator
 * date:          11/2015 (first release), 03/2019 (last update)
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MEM_SIZE			0x00100000
#define SRAM_BASE			0x40000000
#define EXIT_TRAP			0xe0000000

#define IRQ_VECTOR			0xf0000000
#define IRQ_CAUSE			0xf0000010
#define IRQ_MASK			0xf0000020
#define IRQ_STATUS			0xf0000030
#define IRQ_EPC				0xf0000040
#define EXTIO_IN			0xf0000080
#define EXTIO_OUT			0xf0000090
#define DEBUG_ADDR			0xf00000d0

#define S0CAUSE				0xe1000400

#define GPIOCAUSE			0xe1010400
#define GPIOCAUSEINV			0xe1010800
#define GPIOMASK			0xe1010c00

#define PADDR				0xe1014000
#define PAOUT				0xe1014010
#define PAIN				0xe1014020
#define PAININV				0xe1014030
#define PAINMASK			0xe1014040

#define TIMERCAUSE			0xe1020400
#define TIMERCAUSE_INV			0xe1020800
#define TIMERMASK			0xe1020c00

#define TIMER0				0xe1024000
#define TIMER1				0xe1024400
#define TIMER1_PRE			0xe1024410
#define TIMER1_CTC			0xe1024420
#define TIMER1_OCR			0xe1024430

#define UARTCAUSE			0xe1030400
#define UARTCAUSE_INV			0xe1030800
#define UARTMASK			0xe1030c00

#define UART0				0xe1034000
#define UART0_DIV			0xe1034010

#define ntohs(A) ( ((A)>>8) | (((A)&0xff)<<8) )
#define htons(A) ntohs(A)
#define ntohl(A) ( ((A)>>24) | (((A)&0xff0000)>>8) | (((A)&0xff00)<<8) | ((A)<<24) )
#define htonl(A) ntohl(A)

typedef struct {
	int32_t r[32];
	uint32_t pc, pc_next;
	int8_t *mem;
	uint32_t vector, cause, mask, status, status_dly[4], epc, exception;
	uint32_t s0cause;
	uint32_t gpiocause, gpiocause_inv, gpiomask;
	uint32_t paddr, paout, pain, pain_inv, pain_mask;
	uint32_t timercause, timercause_inv, timermask;
	uint32_t timer0, timer1, timer1_pre, timer1_ctc, timer1_ocr;
	uint32_t uartcause, uartcause_inv, uartmask;
	uint64_t cycles;
} state;

int8_t sram[MEM_SIZE];

FILE *fptr;
int32_t log_enabled = 0;

void dumpregs(state *s){
	int32_t i;

	for (i = 0; i < 32; i+=4){
		printf("\nr%02d [%08x] r%02d [%08x] r%02d [%08x] r%02d [%08x]", \
		i, s->r[i], i+1, s->r[i+1], i+2, s->r[i+2], i+3, s->r[i+3]);
	}
	printf("\n");
}

void bp(state *s, uint32_t ir){
	printf("\npc: %08x, ir: %08x", s->pc, ir);
	dumpregs(s);
	getchar();
}

static int32_t mem_fetch(state *s, uint32_t address){
	uint32_t value=0;
	uint32_t *ptr;

	ptr = (uint32_t *)(s->mem + (address % MEM_SIZE));
	value = *ptr;

	return(value);
}

static int32_t mem_read(state *s, int32_t size, uint32_t address){
	uint32_t value=0;
	uint32_t *ptr;

	switch (address){
		case IRQ_VECTOR:	return s->vector;
		case IRQ_CAUSE:		return s->cause;
		case IRQ_MASK:		return s->mask;
		case IRQ_STATUS:	return s->status;
		case IRQ_EPC:		return s->epc;
		case S0CAUSE:		return s->s0cause;
		case GPIOCAUSE:		return s->gpiocause;
		case GPIOCAUSEINV:	return s->gpiocause_inv;
		case GPIOMASK:		return s->gpiomask;
		case PADDR:		return s->paddr;
		case PAOUT:		return s->paout;
		case PAIN:		return s->pain;
		case PAININV:		return s->pain_inv;
		case PAINMASK:		return s->pain_mask;
		case TIMERCAUSE:	return s->timercause;
		case TIMERCAUSE_INV:	return s->timercause_inv;
		case TIMERMASK:		return s->timermask;
		case TIMER0:		return s->timer0;
		case TIMER1:		return s->timer1;
		case TIMER1_PRE:	return s->timer1_pre;
		case TIMER1_CTC:	return s->timer1_ctc;
		case TIMER1_OCR:	return s->timer1_ocr;
		case UARTCAUSE:		return s->uartcause;
		case UARTCAUSE_INV:	return s->uartcause_inv;
		case UARTMASK:		return s->uartmask;
		case UART0:		return getchar();
		case UART0_DIV:		return 0;
	}
	if (address >= EXIT_TRAP) return 0;

	ptr = (uint32_t *)(s->mem + (address % MEM_SIZE));

	switch (size){
		case 4:
			if(address & 3){
				printf("\nunaligned access (load word) pc=0x%x addr=0x%x", s->pc, address);
				dumpregs(s);
				exit(1);
			}else{
				value = *(int32_t *)ptr;
			}
			break;
		case 2:
			if(address & 1){
				printf("\nunaligned access (load halfword) pc=0x%x addr=0x%x", s->pc, address);
				dumpregs(s);
				exit(1);
			}else{
				value = *(int16_t *)ptr;
			}
			break;
		case 1:
			value = *(int8_t *)ptr;
			break;
		default:
			printf("\nerror");
	}

	return(value);
}

static void mem_write(state *s, int32_t size, uint32_t address, uint32_t value){
	uint32_t i;
	uint32_t *ptr;

	switch (address){
		case IRQ_VECTOR:	s->vector = value; return;
		case IRQ_MASK:		s->mask = value; return;
		case IRQ_STATUS:	if (value == 0){ s->status = 0; for (i = 0; i < 4; i++) s->status_dly[i] = 0; }else{ s->status_dly[3] = value; } return;
		case IRQ_EPC:		s->epc = value; return;
		case GPIOCAUSE:		s->gpiocause = value & 0xffff; return;
		case GPIOCAUSEINV:	s->gpiocause_inv = value & 0xffff; return;
		case GPIOMASK:		s->gpiomask = value & 0xffff; return;
		case PADDR:		s->paddr = value & 0xffff; return;
		case PAOUT:		s->paout = value & 0xffff; return;
//		case PAIN:		s->gpiocause = value & 0xffff; return;
		case PAININV:		s->pain_inv = value & 0xffff; return;
		case PAINMASK:		s->pain_mask = value & 0xffff; return;
		case TIMERCAUSE_INV:	s->timercause_inv = value & 0xff; return;
		case TIMERMASK:		s->timermask = value & 0xff; return;
		case TIMER0:		return;
		case TIMER1:		s->timer1 = value & 0xffff; return;
		case TIMER1_PRE:	s->timer1_pre = value & 0xffff; return;
		case TIMER1_CTC:	s->timer1_ctc = value & 0xffff; return;
		case TIMER1_OCR:	s->timer1_ocr = value & 0xffff; return;
		case UARTCAUSE_INV:	s->uartcause_inv = value & 0xff; return;
		case UARTMASK:		s->uartmask = value & 0xff; return;

		case EXIT_TRAP:
			fflush(stdout);
			if (log_enabled)
				fclose(fptr);
			printf("\nend of simulation - %ld cycles.\n", s->cycles);
			exit(0);
		case DEBUG_ADDR:
			if (log_enabled)
				fprintf(fptr, "%c", (int8_t)(value & 0xff));
			return;
		case UART0:
			fprintf(stdout, "%c", (int8_t)(value & 0xff));
			return;
		case UART0_DIV:
			return;
	}
	if (address >= EXIT_TRAP) return;

	ptr = (uint32_t *)(s->mem + (address % MEM_SIZE));

	switch (size){
		case 4:
			if(address & 3){
				printf("\nunaligned access (store word) pc=0x%x addr=0x%x", s->pc, address);
				dumpregs(s);
				exit(1);
			}else{
				*(int32_t *)ptr = value;
			}
			break;
		case 2:
			if(address & 1){
				printf("\nunaligned access (store halfword) pc=0x%x addr=0x%x", s->pc, address);
				dumpregs(s);
				exit(1);
			}else{
				*(int16_t *)ptr = (uint16_t)value;
			}
			break;
		case 1:
			*(int8_t *)ptr = (uint8_t)value;
			break;
		default:
			printf("\nerror");
	}
}

void cycle(state *s){
	uint32_t inst, i;
	uint32_t opcode, rd, rs1, rs2, funct3, funct7, imm_i, imm_s, imm_sb, imm_u, imm_uj;
	int32_t *r = s->r;
	uint32_t *u = (uint32_t *)s->r;
	uint32_t ptr_l, ptr_s;

	if ((s->status && (s->cause & s->mask)) || s->exception){
		s->epc = s->pc_next;
		s->pc = s->vector;
		s->pc_next = s->vector + 4;
		s->status = 0;
		s->exception = 0;
		for (i = 0; i < 4; i++)
			s->status_dly[i] = 0;
	}

	inst = mem_fetch(s, s->pc);

	opcode = inst & 0x7f;
	rd = (inst >> 7) & 0x1f;
	rs1 = (inst >> 15) & 0x1f;
	rs2 = (inst >> 20) & 0x1f;
	funct3 = (inst >> 12) & 0x7;
	funct7 = (inst >> 25) & 0x7f;
	imm_i = (inst & 0xfff00000) >> 20;
	imm_s = ((inst & 0xf80) >> 7) | ((inst & 0xfe000000) >> 20);
	imm_sb = ((inst & 0xf00) >> 7) | ((inst & 0x7e000000) >> 20) | ((inst & 0x80) << 4) | ((inst & 0x80000000) >> 19);
	imm_u = inst & 0xfffff000;
	imm_uj = ((inst & 0x7fe00000) >> 20) | ((inst & 0x100000) >> 9) | (inst & 0xff000) | ((inst & 0x80000000) >> 11);
	if (inst & 0x80000000){
		imm_i |= 0xfffff000;
		imm_s |= 0xfffff000;
		imm_sb |= 0xffffe000;
		imm_uj |= 0xffe00000;
	}
	ptr_l = r[rs1] + (int32_t)imm_i;
	ptr_s = r[rs1] + (int32_t)imm_s;
	r[0] = 0;

	switch (opcode){
		case 0x37: r[rd] = imm_u; break;										/* LUI */
		case 0x17: r[rd] = s->pc + imm_u; break;									/* AUIPC */
		case 0x6f: r[rd] = s->pc_next; s->pc_next = s->pc + imm_uj; break;						/* JAL */
		case 0x67: r[rd] = s->pc_next; s->pc_next = (r[rs1] + imm_i) & 0xfffffffe; break;				/* JALR */
		case 0x63:
			switch (funct3){
				case 0x0: if (r[rs1] == r[rs2]){ s->pc_next = s->pc + imm_sb; } break;				/* BEQ */
				case 0x1: if (r[rs1] != r[rs2]){ s->pc_next = s->pc + imm_sb; } break;				/* BNE */
				case 0x4: if (r[rs1] < r[rs2]){ s->pc_next = s->pc + imm_sb; } break;				/* BLT */
				case 0x5: if (r[rs1] >= r[rs2]){ s->pc_next = s->pc + imm_sb; } break;				/* BGE */
				case 0x6: if (u[rs1] < u[rs2]){ s->pc_next = s->pc + imm_sb; } break;				/* BLTU */
				case 0x7: if (u[rs1] >= u[rs2]){ s->pc_next = s->pc + imm_sb; } break;				/* BGEU */
				default: goto fail;
			}
			break;
		case 0x3:
			switch (funct3){
				case 0x0: r[rd] = (int8_t)mem_read(s,1,ptr_l); break;						/* LB */
				case 0x1: r[rd] = (int16_t)mem_read(s,2,ptr_l); break;						/* LH */
				case 0x2: r[rd] = mem_read(s,4,ptr_l); break;							/* LW */
				case 0x4: r[rd] = (uint8_t)mem_read(s,1,ptr_l); break;						/* LBU */
				case 0x5: r[rd] = (uint16_t)mem_read(s,2,ptr_l); break;						/* LHU */
				default: goto fail;
			}
			break;
		case 0x23:
			switch (funct3){
				case 0x0: mem_write(s,1,ptr_s,r[rs2]); break;							/* SB */
				case 0x1: mem_write(s,2,ptr_s,r[rs2]); break;							/* SH */
				case 0x2: mem_write(s,4,ptr_s,r[rs2]); break;							/* SW */
				default: goto fail;
			}
			break;
		case 0x13:
			switch (funct3){
				case 0x0: r[rd] = r[rs1] + (int32_t)imm_i; break;						/* ADDI */
				case 0x2: r[rd] = r[rs1] < (int32_t)imm_i; break;		 				/* SLTI */
				case 0x3: r[rd] = u[rs1] < (uint32_t)imm_i; break;						/* SLTIU */
				case 0x4: r[rd] = r[rs1] ^ (int32_t)imm_i; break;						/* XORI */
				case 0x6: r[rd] = r[rs1] | (int32_t)imm_i; break;						/* ORI */
				case 0x7: r[rd] = r[rs1] & (int32_t)imm_i; break;						/* ANDI */
				case 0x1: r[rd] = u[rs1] << (rs2 & 0x3f); break;						/* SLLI */
				case 0x5:
					switch (funct7){
						case 0x0: r[rd] = u[rs1] >> (rs2 & 0x3f); break;				/* SRLI */
						case 0x20: r[rd] = r[rs1] >> (rs2 & 0x3f); break;				/* SRAI */
						default: goto fail;
					}
					break;
				default: goto fail;
			}
			break;
		case 0x33:
			switch (funct3){
				case 0x0:
					switch (funct7){
						case 0x0: r[rd] = r[rs1] + r[rs2]; break;					/* ADD */
						case 0x20: r[rd] = r[rs1] - r[rs2]; break;					/* SUB */
						default: goto fail;
					}
					break;
				case 0x1: r[rd] = r[rs1] << r[rs2]; break;							/* SLL */
				case 0x2: r[rd] = r[rs1] < r[rs2]; break;		 					/* SLT */
				case 0x3: r[rd] = u[rs1] < u[rs2]; break;		 					/* SLTU */
				case 0x4: r[rd] = r[rs1] ^ r[rs2]; break;							/* XOR */
				case 0x5:
					switch (funct7){
						case 0x0: r[rd] = u[rs1] >> u[rs2]; break;					/* SRL */
						case 0x20: r[rd] = r[rs1] >> r[rs2]; break;					/* SRA */
						default: goto fail;
					}
					break;
				case 0x6: r[rd] = r[rs1] | r[rs2]; break;							/* OR */
				case 0x7: r[rd] = r[rs1] & r[rs2]; break;							/* AND */
				default: goto fail;
			}
			break;
		case 0x73:
			switch(funct3){
				case 0:
					switch(imm_i){
						case 0:										/* SCALL */
							s->exception = 1;
							break;
						case 1:	bp(s, inst); break;							/* SBREAK */
						default: goto fail;
					}
					break;
				default: goto fail;
			}
			break;
		default: goto fail;
	}

	s->pc = s->pc_next;
	s->pc_next = s->pc_next + 4;
	s->status = s->status_dly[0];
	for (i = 0; i < 3; i++)
		s->status_dly[i] = s->status_dly[i+1];

	s->gpiocause = (s->pain ^ s->pain_inv) & s->pain_mask ? 0x01 : 0x00;
	if (s->timer0 & 0x10000) {
		s->timercause |= 0x01;
	} else {
		s->timercause &= 0xfe;
	}
	if (s->timer0 & 0x40000) {
		s->timercause |= 0x02;
	} else {
		s->timercause &= 0xfd;
	}
	if (s->timer1 == s->timer1_ctc) {
		s->timer1 = 0;
		s->timercause ^= 0x4;
	}
	if (s->timer1 < s->timer1_ocr) {
		s->timercause |= 0x8;
	} else {
		s->timercause &= 0xf7;
	}
	s->s0cause = (s->gpiocause ^ s->gpiocause_inv) & s->gpiomask ? 0x02 : 0x00;
	s->s0cause |= (s->timercause ^ s->timercause_inv) & s->timermask ? 0x04 : 0x00;
	s->cause = s->s0cause ? 0x01 : 0x00;

	s->cycles++;
	s->timer0++;
	switch (s->timer1_pre) {
		case 1:
			if (!(s->timer0 & 3)) s->timer1++;
			break;
		case 2:
			if (!(s->timer0 & 15)) s->timer1++;
			break;
		case 3:
			if (!(s->timer0 & 63)) s->timer1++;
			break;
		case 4:
			if (!(s->timer0 & 255)) s->timer1++;
			break;
		case 5:
			if (!(s->timer0 & 1023)) s->timer1++;
			break;
		case 6:
			if (!(s->timer0 & 4095)) s->timer1++;
			break;
		case 7:
			if (!(s->timer0 & 16383)) s->timer1++;
			break;
		default:
			s->timer1++;
	}
	s->timer1 &= 0xffff;

	return;
fail:
	printf("\ninvalid opcode (pc=0x%x opcode=0x%x)", s->pc, inst);
	exit(0);
}

int main(int argc, char *argv[]){
	state context;
	state *s;
	FILE *in;
	int bytes;

	s = &context;
	memset(s, 0, sizeof(state));
	memset(sram, 0xff, sizeof(MEM_SIZE));

	if (argc >= 2){
		in = fopen(argv[1], "rb");
		if (in == 0){
			printf("\nerror opening binary file.\n");
			return 1;
		}
		bytes = fread(&sram, 1, MEM_SIZE, in);
		fclose(in);
		if (bytes == 0){
			printf("\nerror reading binary file.\n");
			return 1;
		}
		if (argc == 3){
			fptr = fopen(argv[2], "wb");
			if (!fptr){
				printf("\nerror reading binary file.\n");
				return 1;
			}
			log_enabled = 1;
		}
	}else{
		printf("\nsyntax: hf_risc_sim [file.bin] [logfile.txt]\n");
		return 1;
	}

	memset(s, 0, sizeof(context));
	s->pc = SRAM_BASE;
	s->pc_next = s->pc + 4;
	s->mem = &sram[0];
	s->r[2] = MEM_SIZE - 4;
	s->exception = 0;

	for(;;){
		if (s->timer0 & 0x80000) {
			s->pain |= 0x8;
		} else {
			s->pain &= ~0x8;
		}
		cycle(s);
	}

	return 0;
}

