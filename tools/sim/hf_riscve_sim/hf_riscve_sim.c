/* file:          hf_riscve_sim.c
 * description:   HF-RISCV-E simulator
 * date:          01/2022
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>

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

// display: RGB565 (16-bit per pixel)
#define DISPLAY_RAM			0xe0400000	// 0xe0400000 - 0xe07fffff (4MB)
#define DISPLAY_RAM_TOP			0xe0800000	// 0xe0400000 - 0xe07fffff (4MB)
#define DISPLAY_STATUS			0xe0e00010	// bit: 31 - 4 (reserved), 3 - update (blit), 2 - deinitialize, 1 - initialize, 0 - configured
#define DISPLAY_WIDTH			0xe0e00020
#define DISPLAY_HEIGHT			0xe0e00030
#define DISPLAY_SCALE			0xe0e00040

#define DISPLAY_CONFIGURED		(1 << 0)
#define DISPLAY_INITIALIZE		(1 << 1)
#define DISPLAY_DEINITIALIZE		(1 << 2)
#define DISPLAY_UPDATE			(1 << 3)


#define ntohs(A) ( ((A)>>8) | (((A)&0xff)<<8) )
#define htons(A) ntohs(A)
#define ntohl(A) ( ((A)>>24) | (((A)&0xff0000)>>8) | (((A)&0xff00)<<8) | ((A)<<24) )
#define htonl(A) ntohl(A)

struct state_s {
	int32_t r[16];
	uint32_t pc, pc_next;
	int8_t *mem;
	uint64_t cycles;
};

struct intctrl_s {
	uint32_t vector, cause, mask, status, status_dly[4], epc, exception;
};

struct periph_s {
	uint32_t s0cause;
	uint32_t gpiocause, gpiocause_inv, gpiomask;
	uint32_t paddr, paout, pain, pain_inv, pain_mask;
	uint32_t timercause, timercause_inv, timermask;
	uint32_t timer0, timer1, timer1_pre, timer1_ctc, timer1_ocr;
	uint32_t uartcause, uartcause_inv, uartmask;
	int8_t *display_mem;
	uint32_t display_status;
	uint16_t display_width, display_height, display_scale;
};

FILE *fptr;
int32_t log_enabled = 0;

SDL_Window *screen;
SDL_Event event;
const uint8_t *inkeys;
SDL_Renderer *renderer;
SDL_Texture *texture;

void display_status(struct periph_s *per)
{
	if (per->display_status & DISPLAY_CONFIGURED) {
		if (per->display_status & DISPLAY_UPDATE) {
			per->display_status &= ~DISPLAY_UPDATE;
			
			SDL_UpdateTexture(texture, NULL, per->display_mem, per->display_width * sizeof(uint16_t));
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			
			return;
		}
		
		if (per->display_status & DISPLAY_DEINITIALIZE) {
			if (per->display_mem)
				free(per->display_mem);
			per->display_status &= ~DISPLAY_DEINITIALIZE;
			per->display_status &= ~DISPLAY_CONFIGURED;
			
			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(screen);
		}
	} else {
		if (per->display_status & DISPLAY_INITIALIZE) {
			if (per->display_width < 128 || per->display_height < 128)
				return;
			
			SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles");

			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
				fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
				exit(1);
			}
			
			screen = SDL_CreateWindow("RGB565 display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				(int)((float)per->display_width * ((float)per->display_scale / 100.0)), (int)((float)per->display_height * ((float)per->display_scale / 100.0)), 0);

			if (screen == NULL) {
				fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
				exit(1);
			}

			renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING,
				per->display_width, per->display_height);

			SDL_SetRelativeMouseMode(SDL_TRUE);
			
			per->display_mem = (int8_t *)malloc(8 * 1024 * 1024);
			if (!per->display_mem)
				return;			

			per->display_status &= ~DISPLAY_INITIALIZE;
			per->display_status |= DISPLAY_CONFIGURED;
		}
	}
}


void dumpregs(struct state_s *s)
{
	int32_t i;

	for (i = 0; i < 16; i+=4){
		printf("\nr%02d [%08x] r%02d [%08x] r%02d [%08x] r%02d [%08x]", \
		i, s->r[i], i+1, s->r[i+1], i+2, s->r[i+2], i+3, s->r[i+3]);
	}
	printf("\n");
}

void bp(struct state_s *s, uint32_t ir)
{
	printf("\npc: %08x, ir: %08x", s->pc, ir);
	dumpregs(s);
	getchar();
}

static int32_t mem_fetch(struct state_s *s, uint32_t address)
{
	uint32_t value=0;
	uint32_t *ptr;

	ptr = (uint32_t *)(s->mem + (address % MEM_SIZE));
	value = *ptr;

	return(value);
}

static int32_t mem_read(struct state_s *s, struct intctrl_s *ic, struct periph_s *per, int32_t size, uint32_t address)
{
	uint32_t value = 0;
	uint32_t *ptr;

	switch (address & ~0xf){
		case IRQ_VECTOR:	return ic->vector;
		case IRQ_CAUSE:		return ic->cause;
		case IRQ_MASK:		return ic->mask;
		case IRQ_STATUS:	return ic->status;
		case IRQ_EPC:		return ic->epc;
		case S0CAUSE:		return per->s0cause;
		case GPIOCAUSE:		return per->gpiocause;
		case GPIOCAUSEINV:	return per->gpiocause_inv;
		case GPIOMASK:		return per->gpiomask;
		case PADDR:		return per->paddr;
		case PAOUT:		return per->paout;
		case PAIN:		return per->pain;
		case PAININV:		return per->pain_inv;
		case PAINMASK:		return per->pain_mask;
		case TIMERCAUSE:	return per->timercause;
		case TIMERCAUSE_INV:	return per->timercause_inv;
		case TIMERMASK:		return per->timermask;
		case TIMER0:		return per->timer0;
		case TIMER1:		return per->timer1;
		case TIMER1_PRE:	return per->timer1_pre;
		case TIMER1_CTC:	return per->timer1_ctc;
		case TIMER1_OCR:	return per->timer1_ocr;
		case UARTCAUSE:		return per->uartcause;
		case UARTCAUSE_INV:	return per->uartcause_inv;
		case UARTMASK:		return per->uartmask;
		case UART0:		return getchar();
		case UART0_DIV:		return 0;
		case DISPLAY_STATUS:	return per->display_status;
		case DISPLAY_WIDTH:	return per->display_width;
		case DISPLAY_HEIGHT:	return per->display_height;
		case DISPLAY_SCALE:	return per->display_scale;
	}
	if (address == EXIT_TRAP) return 0;

	if (address >= DISPLAY_RAM && address < DISPLAY_RAM_TOP)
		ptr = (uint32_t *)(per->display_mem + (address % (DISPLAY_RAM_TOP - DISPLAY_RAM)));
	else
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

static void mem_write(struct state_s *s, struct intctrl_s *ic, struct periph_s *per, int32_t size, uint32_t address, uint32_t value)
{
	uint32_t i;
	uint32_t *ptr;

	switch (address & ~0xf){
		case IRQ_VECTOR:	ic->vector = value; return;
		case IRQ_MASK:		ic->mask = value; return;
		case IRQ_STATUS:	if (value == 0){ ic->status = 0; for (i = 0; i < 4; i++) ic->status_dly[i] = 0; } else { ic->status_dly[3] = value; } return;
		case IRQ_EPC:		ic->epc = value; return;
		case GPIOCAUSE:		per->gpiocause = value & 0xffff; return;
		case GPIOCAUSEINV:	per->gpiocause_inv = value & 0xffff; return;
		case GPIOMASK:		per->gpiomask = value & 0xffff; return;
		case PADDR:		per->paddr = value & 0xffff; return;
		case PAOUT:		per->paout = value & 0xffff; return;
		case PAININV:		per->pain_inv = value & 0xffff; return;
		case PAINMASK:		per->pain_mask = value & 0xffff; return;
		case TIMERCAUSE_INV:	per->timercause_inv = value & 0xff; return;
		case TIMERMASK:		per->timermask = value & 0xff; return;
		case TIMER0:		return;
		case TIMER1:		per->timer1 = value & 0xffff; return;
		case TIMER1_PRE:	per->timer1_pre = value & 0xffff; return;
		case TIMER1_CTC:	per->timer1_ctc = value & 0xffff; return;
		case TIMER1_OCR:	per->timer1_ocr = value & 0xffff; return;
		case UARTCAUSE_INV:	per->uartcause_inv = value & 0xff; return;
		case UARTMASK:		per->uartmask = value & 0xff; return;
		case DISPLAY_STATUS:	per->display_status = value; display_status(per); return;
		case DISPLAY_WIDTH:	per->display_width = value & 0xffff; return;
		case DISPLAY_HEIGHT:	per->display_height = value & 0xffff; return;
		case DISPLAY_SCALE:	per->display_scale = value & 0xffff; return;

		case EXIT_TRAP:
			fflush(stdout);
			if (log_enabled)
				fclose(fptr);
			printf("\nend of simulation - %ld cycles (%d).\n", s->cycles, s->r[10]);
			exit(s->r[10]);
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
	if (address == EXIT_TRAP) return;

	if (address >= DISPLAY_RAM && address < DISPLAY_RAM_TOP)
		ptr = (uint32_t *)(per->display_mem + (address % (DISPLAY_RAM_TOP - DISPLAY_RAM)));
	else
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


void intctrl_cycle(struct state_s *s, struct intctrl_s *ic, struct periph_s *per)
{
	uint32_t i;
	
	if ((ic->status && (ic->cause & ic->mask)) || ic->exception) {
		ic->epc = s->pc_next;
		s->pc = ic->vector;
		s->pc_next = ic->vector + 4;
		ic->status = 0;
		ic->exception = 0;
		for (i = 0; i < 4; i++)
			ic->status_dly[i] = 0;
	}
	
	ic->status = ic->status_dly[0];
	for (i = 0; i < 3; i++)
		ic->status_dly[i] = ic->status_dly[i+1];
	ic->cause = per->s0cause ? 0x01 : 0x00;
}

void periph_cycle(struct periph_s *per)
{
	per->gpiocause = (per->pain ^ per->pain_inv) & per->pain_mask ? 0x01 : 0x00;
	if (per->timer0 & 0x10000) {
		per->timercause |= 0x01;
	} else {
		per->timercause &= 0xfe;
	}
	if (per->timer0 & 0x40000) {
		per->timercause |= 0x02;
	} else {
		per->timercause &= 0xfd;
	}
	if (per->timer1 == per->timer1_ctc) {
		per->timer1 = 0;
		per->timercause ^= 0x4;
	}
	if (per->timer1 < per->timer1_ocr) {
		per->timercause |= 0x8;
	} else {
		per->timercause &= 0xf7;
	}
	if (per->timer1 & 0x8000) {
		per->timercause |= 0x10;
	} else {
		per->timercause &= 0xef;
	}
	per->s0cause = (per->gpiocause ^ per->gpiocause_inv) & per->gpiomask ? 0x02 : 0x00;
	per->s0cause |= (per->timercause ^ per->timercause_inv) & per->timermask ? 0x04 : 0x00;
	per->timer0++;
	
	switch (per->timer1_pre) {
		case 1:
			if (!(per->timer0 & 3)) per->timer1++;
			break;
		case 2:
			if (!(per->timer0 & 15)) per->timer1++;
			break;
		case 3:
			if (!(per->timer0 & 63)) per->timer1++;
			break;
		case 4:
			if (!(per->timer0 & 255)) per->timer1++;
			break;
		case 5:
			if (!(per->timer0 & 1023)) per->timer1++;
			break;
		case 6:
			if (!(per->timer0 & 4095)) per->timer1++;
			break;
		case 7:
			if (!(per->timer0 & 16383)) per->timer1++;
			break;
		default:
			per->timer1++;
	}
	per->timer1 &= 0xffff;
}

void cpu_cycle(struct state_s *s, struct intctrl_s *ic, struct periph_s *per)
{
	uint32_t inst;
	uint32_t opcode, rd, rs1, rs2, funct3, funct7, imm_i, imm_s, imm_sb, imm_u, imm_uj;
	int32_t *r = s->r;
	uint32_t *u = (uint32_t *)s->r;
	uint32_t ptr_l, ptr_s;

	inst = mem_fetch(s, s->pc);

	opcode = inst & 0x7f;
	rd = (inst >> 7) & 0xf;
	rs1 = (inst >> 15) & 0xf;
	rs2 = (inst >> 20) & 0xf;
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
				case 0x0: r[rd] = (int8_t)mem_read(s, ic, per, 1, ptr_l); break;				/* LB */
				case 0x1: r[rd] = (int16_t)mem_read(s, ic, per, 2, ptr_l); break;				/* LH */
				case 0x2: r[rd] = mem_read(s, ic, per, 4, ptr_l); break;					/* LW */
				case 0x4: r[rd] = (uint8_t)mem_read(s, ic, per, 1, ptr_l); break;				/* LBU */
				case 0x5: r[rd] = (uint16_t)mem_read(s, ic, per, 2, ptr_l); break;				/* LHU */
				default: goto fail;
			}
			break;
		case 0x23:
			switch (funct3){
				case 0x0: mem_write(s, ic, per, 1, ptr_s, r[rs2]); break;					/* SB */
				case 0x1: mem_write(s, ic, per, 2, ptr_s, r[rs2]); break;					/* SH */
				case 0x2: mem_write(s, ic, per, 4, ptr_s,r[rs2]); break;					/* SW */
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
				case 0x1: r[rd] = u[rs1] << (imm_i & 0x3f); break;						/* SLLI */
				case 0x5:
					switch (funct7){
						case 0x0: r[rd] = u[rs1] >> (imm_i & 0x3f); break;				/* SRLI */
						case 0x20: r[rd] = r[rs1] >> (imm_i & 0x3f); break;				/* SRAI */
						default: goto fail;
					}
					break;
				default: goto fail;
			}
			break;
		case 0x33:
			if (funct7 == 0x1){											/* RV32M */
				goto fail;
			}else{
				switch (funct3){
					case 0x0:
						switch (funct7){
							case 0x0: r[rd] = r[rs1] + r[rs2]; break;				/* ADD */
							case 0x20: r[rd] = r[rs1] - r[rs2]; break;				/* SUB */
							default: goto fail;
						}
						break;
					case 0x1: r[rd] = r[rs1] << r[rs2]; break;						/* SLL */
					case 0x2: r[rd] = r[rs1] < r[rs2]; break;		 				/* SLT */
					case 0x3: r[rd] = u[rs1] < u[rs2]; break;		 				/* SLTU */
					case 0x4: r[rd] = r[rs1] ^ r[rs2]; break;						/* XOR */
					case 0x5:
						switch (funct7){
							case 0x0: r[rd] = u[rs1] >> u[rs2]; break;				/* SRL */
							case 0x20: r[rd] = r[rs1] >> r[rs2]; break;				/* SRA */
							default: goto fail;
						}
						break;
					case 0x6: r[rd] = r[rs1] | r[rs2]; break;						/* OR */
					case 0x7: r[rd] = r[rs1] & r[rs2]; break;						/* AND */
					default: goto fail;
				}
				break;
			}
		case 0x73:
			switch(funct3){
				case 0:
					switch(imm_i){
						case 0:										/* SCALL */
							ic->exception = 1;
							break;
						case 1:	bp(s, inst); break;							/* SBREAK */
						default: goto fail;
					}
					break;
				default:											/* CSRxx instructions */
					ic->exception = 1;
					break;
			}
			break;
		default: goto fail;
	}

	s->pc = s->pc_next;
	s->pc_next = s->pc_next + 4;
	s->cycles++;

	return;
fail:
	printf("\ninvalid opcode (pc=0x%x opcode=0x%x)", s->pc, inst);
	exit(0);
}

int main(int argc, char *argv[])
{
	struct state_s context;
	struct intctrl_s interrupt;
	struct periph_s peripheral;
	struct state_s *s = &context;
	struct intctrl_s *ic = &interrupt;
	struct periph_s *per = &peripheral;
	
	int8_t sram[MEM_SIZE];
	FILE *in;
	int bytes;

	memset(s, 0, sizeof(struct state_s));
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
		printf("\nsyntax: hf_riscve_sim [file.bin] [logfile.txt]\n");
		return 1;
	}

	memset(s, 0, sizeof(context));
	s->pc = SRAM_BASE;
	s->pc_next = s->pc + 4;
	s->mem = &sram[0];
	s->r[2] = MEM_SIZE - 4;
	ic->exception = 0;
	per->display_mem = 0;
	per->display_status = 0;
	per->display_width = 0;
	per->display_height = 0;
	per->display_scale = 1;

	for(;;){
		intctrl_cycle(s, ic, per);
		periph_cycle(per);
		cpu_cycle(s, ic, per);

		// simulate an alternating value on port A input
		if (per->timer0 & 0x80000) {
			per->pain |= 0x8;
		} else {
			per->pain &= ~0x8;
		}
	}

	return 0;
}

