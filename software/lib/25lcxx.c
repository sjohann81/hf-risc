/*
all EEPROMs compatible with the Microchip 25lcxx family.
page size should be set according to EEPROM chip in use:
25xx08, 25xx16: 16
25xx32, 25xx64: 32
25xx128, 25xx256: 64
25xx512, 25xx1024: 128
*/

#define PAGE_SIZE	64

#define CMD_READ	0x03
#define CMD_WRITE	0x02
#define CMD_WRDI	0x04
#define CMD_WREN	0x06
#define CMD_RDSR	0x05
#define CMD_WRSR	0x01

#include <hf-risc.h>

uint8_t spi_eeprom_readbyte(uint16_t addr){
	uint8_t data;
	
	spi_start();
	spi_transfer(CMD_READ);
	spi_transfer(addr >> 8);
	spi_transfer(addr & 0xff);
	data = spi_transfer(0);
	spi_stop();
	
	return data;
}

void spi_eeprom_read(uint16_t addr, uint8_t *buf, uint16_t size){
	uint8_t data;
	uint16_t i;
	
	spi_start();
	spi_transfer(CMD_READ);
	spi_transfer(addr >> 8);
	spi_transfer(addr & 0xff);
	for(i = 0; i < size; i++)
		buf[i] = spi_transfer(0);
	spi_stop();
}

void spi_eeprom_writepage(uint16_t page, uint8_t *data){
	uint16_t i;
	
	spi_start();
	spi_transfer(CMD_WREN);
	spi_stop();
	spi_start();
	spi_transfer(CMD_WRITE);
	spi_transfer((page * PAGE_SIZE) >> 8);
	spi_transfer((page * PAGE_SIZE) & 0xff);
	for (i = 0; i < PAGE_SIZE; i++)
		spi_transfer(data[i]);
	spi_stop();
	delay_ms(10);
}
