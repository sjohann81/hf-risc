#include <hf-risc.h>

/*
monitor auxiliry routines
TODO: use libc routines?
*/

uint32_t getnum(void){
	int32_t i;
	uint32_t ch, ch2, value=0;

	for(i = 0; i < 16; ){
		ch = ch2 = getchar();
		if(ch == '\n' || ch == '\r')
			break;
		if('0' <= ch && ch <= '9')
			ch -= '0';
		else if('A' <= ch && ch <= 'Z')
			ch = ch - 'A' + 10;
		else if('a' <= ch && ch <= 'z')
			ch = ch - 'a' + 10;
		else if(ch == 8){
			if(i > 0){
				--i;
				putchar(ch);
				putchar(' ');
				putchar(ch);
			}
			value >>= 4;
			continue;
		}
		putchar(ch2);
		value = (value << 4) + ch;
		++i;
	}
	return value;
}

void printnum(uint32_t n){
	int8_t buf[30];
	uint16_t i;

	itoa(n, buf, 10);
	i = 0;
	while (buf[i]) putchar(buf[i++]);
}

void printhex(uint32_t n){
	int8_t buf[30];
	uint16_t i;

	if (n < 16) putchar('0');
	itoa(n, buf, 16);
	i = 0;
	while (buf[i]) putchar(buf[i++]);
}

int32_t printstr(const int8_t *str){
	while(*str)
		putchar(*str++);

	return 0;
}

int32_t main(void){
	int32_t i, j, k, l, ch;
	uint32_t addr, value;
	void (*funcPtr)();
	uint8_t *ptr1;
	uint16_t d;

	d = (uint16_t)(CPU_SPEED / 57600);
	UART0DIV = d;
	UART0 = 0;

	PADDR |= MASK_P2;
	PADDR &= ~MASK_P3;
	PAALTCFG0 |= MASK_UART0;

	printstr("\nHF-RISC bootloader - ");
	printstr(__DATE__);
	printstr("\n");

	for(;;){
		printstr("\n[u, U] upload binary");
		printstr("\n[b, B] boot ");
		printstr("\n[d   ] mem dump");
		printstr("\n[f   ] mem fill");
		printstr("\n[w   ] write data");
		printstr("\n");
		ch = getchar();
		getchar();

		switch(ch){
// TODO: init cache!
			case 'u':
				ptr1 = (uint8_t *)RAM_BASE;
				goto waiting;
			case 'U':
				printstr("\naddress (hex):");
				addr = getnum();
				ptr1 = (uint8_t *)addr;
			waiting:
				printstr("\nwaiting for binary...");
				ch = getchar();
				for(i = 0; ; i++){
					ptr1[i] = (uint8_t)ch;
					for(j = 0; j < (CPU_SPEED / 100); j++){
						if(kbhit()){
							ch = getchar();
							break;
						}
					}
					if (j >= (CPU_SPEED / 100)) break;
					if ((i % 1024) == 0) putchar('*');
				}
				i++;
				printstr("--> ");
				printnum(i);
				printstr(" bytes");

// TODO: set the stack pointer to the end of the according region (e.g. scratch pad, external RAM ...)
				break;
			case 'b':
				funcPtr = (void (*)(void *))(RAM_BASE);
				goto booting;
			case 'B':
				printstr("\naddress (hex):");
				addr = getnum();
				funcPtr = (void (*)(void *))addr;
			booting:
				printstr("\nboot\n");
				funcPtr((void *)0);
				break;
			case 'd':
// TODO: use hexdump()
				printstr("\naddress (hex):");
				addr = getnum();
				ptr1 = (uint8_t *)addr;
				printstr("\nlength (hex):");
				i = getnum();
				for(k = 0; k < i; k += 16){
					printstr("\n");
					printhex(addr + k);
					printstr("  ");
					for(l = 0; l < 16; l++){
						printhex(ptr1[k+l]);
						printstr(" ");
						if (l == 7) printstr(" ");
					}
					printstr(" |");
					for(l = 0; l < 16; l++){
						ch = ptr1[k+l];
						if ((ch >= 32) && (ch <= 126))
							putchar((uint8_t)ch);
						else
							printstr(".");
					}
					printstr("|");
				}
				break;
			case 'f':
				printstr("\naddress (hex):");
				addr = getnum();
				ptr1 = (uint8_t *)addr;
				printstr("\nlength (hex):");
				i = getnum();
				printstr("\nbyte (hex):");
				ch = (uint8_t)getnum();
				for (l = 0; l < i; l++)
					ptr1[l] = ch;
				break;
			case 'w':
				printstr("\naddress (hex):");
				addr = getnum();
				printstr("\ndata (hex):");
				value = getnum();
				*(volatile uint32_t *)addr = value;
				break;
			default:
				break;
		}
	}

	return 0;
}

