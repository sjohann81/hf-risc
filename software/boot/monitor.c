#include <hf-risc.h>

/* memory mapped I/O access */
#define MemoryRead(A)			(*(volatile unsigned int*)(A))
#define MemoryWrite(A,V)		*(volatile unsigned int*)(A)=(V)

/* SPI interface - EXTIO_OUT pins 0 (chip select), 1 (clock), 2 (master output) and 3 (chip select 2) */
#define SPI_CS		0x01
#define SPI_SCK		0x02
#define SPI_MOSI	0x04
#define SPI_CS2		0x08

/* SPI interface - EXIO_IN pin 3 (master input) */
#define SPI_MISO	0x08

/* EEPROM config / commands */
#define PAGE_SIZE	64
#define CMD_READ	0x03
#define CMD_WRITE	0x02
#define CMD_WREN	0x06

/*
monitor auxiliry routines
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

/*
SPI low level interface driver, mode 0
*/

void spi_setup(void){
	EXTIO_OUT |= SPI_CS | SPI_CS2;
	EXTIO_OUT &= ~SPI_SCK;
}

void spi_start(void){
	EXTIO_OUT &= ~SPI_CS2;
}

void spi_stop(void){
	EXTIO_OUT |= SPI_CS2;
}

int8_t spi_sendrecv(int8_t data){
	int32_t i;
	int8_t newdata = 0;

	for (i = 0; i < 8; i++){
		if (data & 0x80){
			EXTIO_OUT |= SPI_MOSI;
		}else{
			EXTIO_OUT &= ~SPI_MOSI;
		}
		data <<= 1;
		EXTIO_OUT |= SPI_SCK;
		newdata <<= 1;
		newdata |= ((EXTIO_IN & SPI_MISO) ? 1 : 0);
		EXTIO_OUT &= ~SPI_SCK;
	}
	return newdata;
}

/*
SPI EEPROM driver
EEPROM should be a 25lc128 (16KB) or 25lc256 (32KB) compatible chip!
if you need a different ROM size / configuration, change PAGE_SIZE accordingly.
*/

uint8_t spi_eeprom_readbyte(uint16_t addr){
	uint8_t data;
	
	spi_start();
	spi_sendrecv(CMD_READ);
	spi_sendrecv(addr >> 8);
	spi_sendrecv(addr & 0xff);
	data = spi_sendrecv(0);
	spi_stop();
	
	return data;
}

void spi_eeprom_read(uint16_t addr, uint8_t *buf, uint16_t size){
	uint8_t data;
	uint16_t i;
	
	spi_start();
	spi_sendrecv(CMD_READ);
	spi_sendrecv(addr >> 8);
	spi_sendrecv(addr & 0xff);
	for(i = 0; i < size; i++)
		buf[i] = spi_sendrecv(0);
	spi_stop();
}

void spi_eeprom_writepage(uint16_t page, uint8_t *data){
	uint16_t i;
	
	spi_start();
	spi_sendrecv(CMD_WREN);
	spi_stop();
	spi_start();
	spi_sendrecv(CMD_WRITE);
	spi_sendrecv((page * PAGE_SIZE) >> 8);
	spi_sendrecv((page * PAGE_SIZE) & 0xff);
	for (i = 0; i < PAGE_SIZE; i++)
		spi_sendrecv(data[i]);
	spi_stop();
	delay_ms(10);
}

void boot_spi(void){
	uint32_t signature, size;
	uint8_t *ptr1;
	funcptr funcPtr;
/*	
	-initialize SPI, mode 0;
	-try to read a 4 byte signature (0xb16b00b5). on fail (no eeprom, no valid data or no jumper) return to monitor;
	-read image size on next 4 bytes (e.g: 2350 bytes will be 0x0000092e;
	-copy ROM data to RAM, starting at 0x40000000
	-jump to 0x40000000
*/	
	spi_setup();
	signature = (spi_eeprom_readbyte(0) << 24) | (spi_eeprom_readbyte(1) << 16) | (spi_eeprom_readbyte(2) << 8) | spi_eeprom_readbyte(3);
	if (signature != 0xb16b00b5) return;
	size = (spi_eeprom_readbyte(4) << 24) | (spi_eeprom_readbyte(5) << 16) | (spi_eeprom_readbyte(6) << 8) | spi_eeprom_readbyte(7);
	ptr1 = (uint8_t *)ADDR_RAM_BASE;
	// skip header and copy EEPROM data to RAM
	spi_eeprom_read(8, ptr1, size);
	funcPtr = (funcptr)ADDR_RAM_BASE;
	funcPtr((void *)0);
}

/*
monitor main program.
we are using the last portion of SRAM as the program stack!
*/

int32_t main(void){
	int32_t i, j, k, l, ch;
	uint32_t addr, addr2, value, size = 0;
	funcptr funcPtr;
	uint32_t *ptr;
	uint8_t *ptr1;
	uint8_t eeprom_page[PAGE_SIZE];

	uart_init(57600);

	boot_spi();

	printstr("\nHF-RISC bootloader - ");
	printstr(__DATE__);
	printstr("\n");

	for(;;){
		printstr("\n[u, U] upload binary");
		printstr("\n[b, B] boot ");
		printstr("\n[P, D] program / dump EEPROM");
		printstr("\n[d   ] mem dump");
		printstr("\n[f   ] mem fill");
		printstr("\n[w   ] write data");
		printstr("\n");
		ch = getchar();
		getchar();

		switch(ch){
			case 'u':
				ptr1 = (uint8_t *)ADDR_RAM_BASE;
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
				size = i;
				break;
			case 'b':
				funcPtr = (funcptr)ADDR_RAM_BASE;
				goto booting;
			case 'B':
				printstr("\naddress (hex):");
				addr = getnum();
				funcPtr = (funcptr)addr;
			booting:
				printstr("\nboot\n");
				funcPtr((void *)0);
				break;
			case 'P':
				if (!size) break;
				ptr1 = (uint8_t *)ADDR_RAM_BASE;
				printstr("\nprogramming "); printnum(size); printstr(" bytes");
				// signature
				eeprom_page[0] = 0xb1; eeprom_page[1] = 0x6b; eeprom_page[2] = 0x00; eeprom_page[3] = 0xb5;
				// image size
				eeprom_page[4] = (size >> 24) & 0xff;
				eeprom_page[5] = (size >> 16) & 0xff;
				eeprom_page[6] = (size >> 8) & 0xff;
				eeprom_page[7] = size & 0xff;
				
				// first page
				i = 0; k = 0;
				for (j = 8; j < PAGE_SIZE; j++){
					if (i < size){
						eeprom_page[j] = ptr1[i++];
					}else{
						eeprom_page[j] = 0x00;
					}
				}
				spi_eeprom_writepage(k, eeprom_page);
				// next pages
				while(i < size){
					k++;
					for (j = 0; j < PAGE_SIZE; j++){
						if (i < size){
							eeprom_page[j] = ptr1[i++];
						}else{
							eeprom_page[j] = 0x00;
						}
					}
					spi_eeprom_writepage(k, eeprom_page);
				}
				break;
			case 'D':
				printstr("\nlength (hex):");
				i = getnum();
				for(k = 0; k < i; k += 16){
					printstr("\n");
					printhex(k);
					printstr("  ");
					for(l = 0; l < 16; l++){
						j = spi_eeprom_readbyte(k + l);
						printhex(j);
						printstr(" ");
						if (l == 7) printstr(" ");
					}
					printstr(" |");
					for(l = 0; l < 16; l++){
						ch = spi_eeprom_readbyte(k + l);
						if ((ch >= 32) && (ch <= 126))
							putchar((uint8_t)ch);
						else
							printstr(".");
					}
					printstr("|");
				}
				break;
			case 'd':
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
				MemoryWrite(addr, value);
				break;
			default:
				break;
		}
	}

	return 0;
}

