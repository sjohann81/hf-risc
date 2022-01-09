#include <hf-risc.h>

void setup_uart(void)
{
	uint16_t d;
	
	d = (uint16_t)(CPU_SPEED / 57600);
	UART0DIV = d;
	UART0 = 0;

	PADDR |= MASK_P2;
	PADDR &= ~MASK_P3;
	PAALTCFG0 |= MASK_UART0;	
}

void boot_eeprom(void)
{
	volatile uint32_t *eeprom;
	void (*fptr)();
/*	
 	eeprom = (uint32_t *)EXT_EEPROM;	
	fptr = (void (*)(void *))(EXT_EEPROM);
	if (eeprom[0xe0 >> 2] == ntohl(0xb16b00b5))
		fptr((void *)0);
*/
	volatile uint32_t *sram;
	sram = (uint32_t *)RAM_BASE;
	eeprom = (uint32_t *)EXT_EEPROM;	
	fptr = (void (*)(void *))(RAM_BASE);
	if (eeprom[0xe0 >> 2] == ntohl(0xb16b00b5)) {
		memcpy((void *)sram, (void *)eeprom, 7936);
		fptr((void *)0);
	}
}

void boot_loop(void)
{
	int32_t i, j, k = 0, ch;
	uint32_t addr, value;
	void (*fptr)();
	int8_t *ptr, *ptr2;
	volatile uint32_t *eeprom, *ptr3;
	uint8_t *wren;
	int8_t buf[80];
	
	ptr = (int8_t *)RAM_BASE;
	fptr = (void (*)(void *))(RAM_BASE);

	printf("\nHF-RISC SoC bootloader - %s\n", __DATE__);

	for (;;) {
		printf("\n[s, e] select SPM / ext SRAM");
		printf("\n[u, U] upload binary");
		printf("\n[b, B] boot");
		printf("\n[P   ] program ext EEPROM");
		printf("\n[d, f] hexdump / fill");
		printf("\n[r, w] read word / write word\n");

		ch = getchar();
		getchar();

		switch(ch) {
		case 's':
			ptr = (int8_t *)RAM_BASE;
			fptr = (void (*)(void *))(RAM_BASE);
			printf("\nSPM selected");
			break;
		case 'e':
			ptr = (int8_t *)EXT_SRAM;
			fptr = (void (*)(void *))(EXT_SRAM);
			printf("\nSRAM selected");
			break;
		case 'U':
			printf("\naddress (hex):");
			getline(buf);
			addr = strtol(buf, 0, 16);
			ptr = (int8_t *)addr;
			fptr = (void (*)(void *))(addr);
		case 'u':
			printf("\nwaiting for binary at 0x%08x...\n", (uint32_t)ptr);
			ch = getchar();
			for (k = 0; ; k++){
				ptr[k] = (int8_t)ch;
				for (j = 0; j < (CPU_SPEED / 100); j++) {
					if (kbhit()){
						ch = getchar();
						break;
					}
				}
				if (j >= (CPU_SPEED / 100)) break;
				if ((k & 0x3ff) == 0) putchar(':');
			}
			k++;
			printf("--> %d bytes", k);
			break;
		case 'B':
			printf("\naddress (hex):");
			getline(buf);
			addr = strtol(buf, 0, 16);
			fptr = (void (*)(void *))addr;
		case 'b':
			printf("\nboot\n");
			fptr((void *)0);
			break;
		case 'P':
			if (!k) {
				printf("\nno data");
				break;
			}
			printf("\nwrite to EEPROM?\n");
			ch = getchar();
			if (ch == 'y' || ch == 'Y') {
				eeprom = (uint32_t *)EXT_EEPROM_UNCACHED;
				wren = (uint8_t *)EXT_EEPROM_WREN;
				ptr3 = (uint32_t *)ptr;
				for (i = 0; i < (k >> 2) + 1; i++) {
					*wren = 0x00;
					eeprom[i] = ptr3[i];
					delay_ms(5);
					if ((i & 0xff) == 0) putchar('#');
				}
				printf("--> %d bytes", k);
			}
			break;
		case 'd':
			printf("\naddress (hex):");
			getline(buf);
			addr = strtol(buf, 0, 16);
			ptr2 = (int8_t *)addr;
			printf("\nlength (hex):");
			getline(buf);
			i = strtol(buf, 0, 16);
			hexdump(ptr2, i);
			break;
		case 'f':
			printf("\naddress (hex):");
			getline(buf);
			addr = strtol(buf, 0, 16);
			ptr2 = (int8_t *)addr;
			printf("\nlength (hex):");
			getline(buf);
			i = strtol(buf, 0, 16);
			printf("\nbyte (hex):");
			getline(buf);
			ch = (int8_t)strtol(buf, 0, 16);
			memset(ptr2, ch, i);
			break;
		case 'r':
			printf("\naddress (hex):");
			getline(buf);
			addr = strtol(buf, 0, 16);
			ptr2 = (int8_t *)addr;
			printf("%08x\n", *ptr2);
			break;
		case 'w':
			printf("\naddress (hex):");
			getline(buf);
			addr = strtol(buf, 0, 16);
			ptr2 = (int8_t *)addr;
			printf("\nword (hex):");
			getline(buf);
			value = strtol(buf, 0, 16);
			*ptr2 = value;
			break;
		default:
			break;
		}
	}	
}

int main(void){
	setup_uart();
	boot_eeprom();
	boot_loop();

	return 0;
}
